  #include <CIrrOdeRePlayer.h>
  #include <CIrrOdeEventFactory.h>

  #include <event/IIrrOdeEvent.h>
  #include <event/CIrrOdeEventIdentify.h>
  #include <event/CIrrOdeEventLoadScene.h>
  #include <event/IIrrOdeEventQueue.h>
  #include <CIrrOdeManager.h>

CIrrOdeRePlayer::CIrrOdeRePlayer(irr::IrrlichtDevice *pDevice, const irr::c8 *sReplay) {
  m_pDevice=pDevice;
  m_pSmgr=pDevice->getSceneManager();
  m_pTimer=pDevice->getTimer();
  m_sReplay=sReplay;
  m_iLastStep=0;
  m_iTimeBuffer=0;

  clearEventList();
  irr::io::IFileSystem *pFs=m_pDevice->getFileSystem();
  irr::io::IReadFile *f=pFs->createAndOpenFile(irr::io::path(sReplay));
  m_bValidReplay=false;

  if (f==NULL) {
    printf("can't open \"%s\"\n",sReplay);
    return;
  }

  CIrrOdeEventFactory *pFactory=CIrrOdeEventFactory::getSharedEventFactory();

  while (f->getPos()<f->getSize()) {
    c8 pBuffer[4096];
    u32 iMessageSize;

    f->read(&iMessageSize,sizeof(u32));
    f->read(pBuffer,iMessageSize);

    m_cSerializer.setBuffer(pBuffer,iMessageSize);
    IIrrOdeEvent *evt=pFactory->createMessage(&m_cSerializer,m_pSmgr);

    if (evt) {
      m_lEvents.push_back(evt);
      if (!m_bValidReplay) {
        if (evt->getType()==irr::ode::eIrrOdeEventIdentify) {
          irr::ode::CIrrOdeEventIdentify *p=(irr::ode::CIrrOdeEventIdentify *)evt;
          m_bValidReplay=!strcmp(p->getIdentify(),_IRR_ODE_IDENTIFY_STRING);
        }
      }

      if (evt->getType()==irr::ode::eIrrOdeEventLoadScene) {
        irr::ode::CIrrOdeEventLoadScene *p=(irr::ode::CIrrOdeEventLoadScene *)evt;
        m_sSceneFile=stringc(p->getScene());
        m_pSmgr->loadScene(m_sSceneFile.c_str());
        printf("\n\t\tscene file=\"%s\"\n\n",m_sSceneFile.c_str());
      }
    }
  }

  f->drop();

  m_iIt=m_lEvents.begin();
  if (!m_bValidReplay) {
    printf("ERROR: \"%s\" is not a valid replay!\n",sReplay);
    clearEventList();
    return;
  }

  irr::ode::CIrrOdeManager::getSharedInstance()->initODE();
  irr::ode::CIrrOdeManager::getSharedInstance()->initPhysics();

  while (m_pTimer->isStopped()) m_pTimer->start();
}

CIrrOdeRePlayer::~CIrrOdeRePlayer() {
  clearEventList();
}

void CIrrOdeRePlayer::update() {
  u32 iThisTime=m_pTimer->getTime();

  if (m_iLastStep!=0) {
    m_iTimeBuffer+=iThisTime-m_iLastStep;

    while (m_iTimeBuffer>16 && m_iIt!=m_lEvents.end()) {
      bool bStep=false;
      do {
        irr::ode::IIrrOdeEvent *p=(irr::ode::IIrrOdeEvent *)*m_iIt,p2=p->clone();
        irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p2);
        if (p->getType()==irr::ode::eIrrOdeEventStep) bStep=true;
        m_iIt++;
        printf("*");
      }
      while (!bStep && m_iIt!=m_lEvents.end());
      m_iTimeBuffer-=16;
    }
    printf("\n");
  }

  m_iLastStep=iThisTime;
}

void CIrrOdeRePlayer::clearEventList() {
  while (m_lEvents.getSize()>0) {
    irr::core::list<irr::ode::IIrrOdeEvent *>::Iterator it=m_lEvents.begin();
    irr::ode::IIrrOdeEvent *p=*it;
    m_lEvents.erase(it);
    delete p;
  }
}
