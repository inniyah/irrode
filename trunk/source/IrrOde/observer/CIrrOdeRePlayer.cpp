  #include <observer/CIrrOdeRePlayer.h>
  #include <observer/CIrrOdeEventFactory.h>

  #include <event/IIrrOdeEvent.h>
  #include <event/CIrrOdeEventIdentify.h>
  #include <event/CIrrOdeEventLoadScene.h>
  #include <event/IIrrOdeEventQueue.h>
  #include <event/CIrrOdeEventBodyRemoved.h>
  #include <event/CIrrOdeEventNodeCloned.h>
  #include <CIrrOdeManager.h>

namespace irr {
namespace ode {

CIrrOdeRePlayer::CIrrOdeRePlayer(irr::IrrlichtDevice *pDevice, const irr::c8 *sReplay) {
  m_pDevice=pDevice;
  m_pSmgr=pDevice->getSceneManager();
  m_pTimer=pDevice->getTimer();
  m_sReplay=sReplay;
  m_iLastStep=0;
  m_iTimeBuffer=0;

  m_bIsPaused=false;
  m_bReplayFinished=false;

  clearEventList();
  irr::io::IFileSystem *pFs=m_pDevice->getFileSystem();
  irr::io::IReadFile *f=pFs->createAndOpenFile(irr::io::path(sReplay));
  m_bValidReplay=false;

  if (f==NULL) {
    printf("can't open \"%s\"\n",sReplay);
    return;
  }

  CIrrOdeEventFactory *pFactory=CIrrOdeEventFactory::getSharedEventFactory();
  m_iNumberOfSteps = 0;

  while (f->getPos()<f->getSize()) {
    c8 pBuffer[4096];
    u32 iMessageSize;

    f->read(&iMessageSize,sizeof(u32));
    f->read(pBuffer,iMessageSize);

    m_cSerializer.setBuffer(pBuffer,iMessageSize);
    IIrrOdeEvent *evt=pFactory->createMessage(&m_cSerializer,m_pSmgr);

    if (evt) {
      if (evt->getType() == irr::ode::eIrrOdeEventStep) m_iNumberOfSteps++;

      m_lEvents.push_back(evt);
      if (!m_bValidReplay) {
        if (evt->getType()==eIrrOdeEventIdentify) {
          CIrrOdeEventIdentify *p=(CIrrOdeEventIdentify *)evt;
          m_bValidReplay=!strcmp(p->getIdentify(),_IRR_ODE_IDENTIFY_STRING);
        }
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

  CIrrOdeManager::getSharedInstance()->initODE();

  while (m_pTimer->isStopped()) m_pTimer->start();
}

CIrrOdeRePlayer::~CIrrOdeRePlayer() {
  clearEventList();
}

void CIrrOdeRePlayer::initWorld(irr::scene::ISceneNode *pNode) {
  if (pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_WORLD_ID) {
    m_pWorld = reinterpret_cast<CIrrOdeWorld *>(pNode);
    return;
  }

  irr::core::list<irr::scene::ISceneNode *> children = pNode->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it = children.begin(); it != children.end(); it++) initWorld(*it);
}

void CIrrOdeRePlayer::update() {
  u32 iThisTime=m_pTimer->getTime();

  if (m_bIsPaused && m_bSceneLoaded) {
    m_iLastStep=iThisTime;
    return;
  }

  if (m_iLastStep!=0) {
    m_iTimeBuffer+=iThisTime-m_iLastStep;

    while (m_iTimeBuffer>16 && m_iIt!=m_lEvents.end()) {
      bool bStep=false;
      do {
        IIrrOdeEvent *p=(IIrrOdeEvent *)*m_iIt,*p2=p->clone();
        if (p->getType()==eIrrOdeEventLoadScene) {
          CIrrOdeEventLoadScene *pEvt=(CIrrOdeEventLoadScene *)p;
          m_sSceneFile=irr::core::stringc(pEvt->getScene());
          m_bSceneLoaded=m_pSmgr->loadScene(m_sSceneFile.c_str());
          CIrrOdeManager::getSharedInstance()->initPhysics();
          printf("scene file: \"%s\"\n",m_sSceneFile.c_str());
        }

        if (p->getType()==eIrrOdeEventNodeCloned) {
          CIrrOdeEventNodeCloned *pClone=(CIrrOdeEventNodeCloned *)p;
          irr::scene::ISceneNode *pNode=m_pSmgr->getSceneNodeFromId(pClone->getSourceId());
          if (pNode && m_pWorld) {
            m_pWorld->cloneOdeNode(pNode,m_pSmgr->getRootSceneNode(),m_pSmgr,pClone->getNewId());
          }
        }

        //printf("%s\n",p2->toString());
        if (p->getType()==eIrrOdeEventStep) bStep=true;
        CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p2);
        m_iIt++;
      }
      while (!bStep && m_iIt!=m_lEvents.end());
      m_iTimeBuffer-=16;
    }
    if (m_iIt==m_lEvents.end()) m_bReplayFinished=true;
  }

  m_iLastStep=iThisTime;
}

void CIrrOdeRePlayer::clearEventList() {
  while (m_lEvents.getSize()>0) {
    irr::core::list<IIrrOdeEvent *>::Iterator it=m_lEvents.begin();
    IIrrOdeEvent *p=*it;
    m_lEvents.erase(it);
    delete p;
  }
}

}
}
