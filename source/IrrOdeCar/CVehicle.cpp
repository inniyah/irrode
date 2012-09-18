  #include <CVehicle.h>

  #include <CPlane.h>
  #include <CTank.h>
  #include <CHeli.h>
  #include <CCar.h>

  #include <CControlEvents.h>
  #include <CCockpitPlane.h>
  #include <CCockpitCar.h>
  #include <CRearView.h>

void CVehicle::removeFromScene(irr::scene::ISceneNode *pNode, irr::ode::CIrrOdeWorld *pWorld) {
  pWorld->removeTreeFromPhysics(pNode);
  irr::s32 iNodeId=pNode->getID();
  pNode->remove();

  irr::ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
}

void CVehicle::fillBodyList(irr::core::list<irr::scene::ISceneNode *> &aVehicles, irr::scene::ISceneNode *pNode, const irr::c8 *sClassName, irr::u32 iMax, irr::ode::CIrrOdeWorld *pWorld) {
  if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
    irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
    if (p->getOdeClassname().equals_ignore_case(sClassName)) {
      printf("%s found (%i)\n",sClassName,aVehicles.size());
      if (aVehicles.size()<iMax)
        aVehicles.push_back(pNode);
      else {
        removeFromScene(pNode, pWorld);
        return;
      }
    }
  }

  irr::core::list<irr::scene::ISceneNode *> children=pNode->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++) fillBodyList(aVehicles,*it,sClassName,iMax, pWorld);
}

CVehicle::CVehicle(irr::IrrlichtDevice *pDevice, irr::u32 iNumCars, irr::u32 iNumPlanes, irr::u32 iNumHelis, irr::u32 iNumTanks, irr::ode::CIrrOdeWorld *pWorld, bool bRearView, irr::ode::IIrrOdeEventQueue *pInputQueue) {
  m_pDevice = pDevice;
  m_pWorld = pWorld;

  m_pInputQueue = pInputQueue;
  m_pInputQueue->addEventListener(this);

  m_pSmgr = pDevice->getSceneManager();

  fillBodyList(m_lCars  ,m_pSmgr->getRootSceneNode(),"car"  ,iNumCars  , m_pWorld);
  fillBodyList(m_lPlanes,m_pSmgr->getRootSceneNode(),"plane",iNumPlanes, m_pWorld);
  fillBodyList(m_lTanks ,m_pSmgr->getRootSceneNode(),"tank" ,iNumTanks , m_pWorld);
  fillBodyList(m_lHelis ,m_pSmgr->getRootSceneNode(),"heli" ,iNumHelis , m_pWorld);

  printf("\nvehicles found:\n\ncars: %i (%i)\nplanes: %i (%i)\nhelicopters: %i (%i)\ntanks: %i (%i)\n\n",m_lCars.size(),iNumCars,m_lPlanes.size(),iNumPlanes,m_lHelis.size(),iNumHelis,m_lTanks.size(),iNumTanks);

  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=m_lPlanes.begin(); it!=m_lPlanes.end(); it++) {
    CPlane *p=new CPlane(m_pDevice, *it, pInputQueue);
    //m_lCockpits.push_back(pCockpit);
    m_lVehicles.push_back(p);
  }

  for (it=m_lCars.begin(); it!=m_lCars.end(); it++) {
    CCar *p=new CCar(m_pDevice,*it, pInputQueue);
    //m_lCockpits.push_back(pCarCockpit);
    m_lVehicles.push_back(p);
  }

  for (it=m_lTanks.begin(); it!=m_lTanks.end(); it++) {
    CTank *p=new CTank(m_pDevice,*it, pInputQueue);
    m_lVehicles.push_back(p);
  }

  for (it=m_lHelis.begin(); it!=m_lHelis.end(); it++) {
    CHeli *p=new CHeli(m_pDevice,*it, pInputQueue);
    //m_lCockpits.push_back(pCockpit);
    m_lVehicles.push_back(p);
  }

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
}

CVehicle::~CVehicle() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
  m_pInputQueue->removeEventListener(this);
}

bool CVehicle::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType() == irr::ode::eIrrOdeEventStep) {
    irr::core::list<IRenderToTexture *>::Iterator it;
    for (it = m_lCockpits.begin(); it!=m_lCockpits.end(); it++) (*it)->update();
  }

  if (pEvent->getType() == eCtrlMsgRequestVehicle) {
    CRequestVehicle *pEvt = reinterpret_cast<CRequestVehicle *>(pEvent);
    printf("Client %i requests vehicle %i\n", pEvt->getClient(), pEvt->getNode());

    irr::core::list<CIrrOdeCarState *>::Iterator it;
    for (it = m_lVehicles.begin(); it != m_lVehicles.end(); it++) {
      CIrrOdeCarState *p = *it;
      if (p->getBody() != NULL && p->getBody()->getID() == pEvt->getNode()) {
        printf("Vehicle is \"%s\"\n", p->getBody()->getName());

        if (p->getControlledBy() == -1) {
          p->setControlledBy(pEvt->getClient());
          CVehicleApproved *pOk = new CVehicleApproved();
          pOk->setNode  (pEvt->getNode  ());
          pOk->setClient(pEvt->getClient());

          irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(pOk);
          return false;
        }
        else printf("Sorry ... vehicle is occupied by %i.\n", p->getControlledBy());
      }
    }
  }

  if (pEvent->getType() == eCtrlMsgLeaveVehicle) {
    CLeaveVehicle *pEvt = reinterpret_cast<CLeaveVehicle *>(pEvent);
    if (pEvt->getAnswer() == 0) {
      printf("%i leaves vehicle %i\n", pEvt->getClient(), pEvt->getNode());
      irr::core::list<CIrrOdeCarState *>::Iterator it;
      for (it = m_lVehicles.begin(); it != m_lVehicles.end(); it++) {
        CIrrOdeCarState *p = *it;
        if (p->getBody() != NULL && p->getBody()->getID() == pEvt->getNode()) {
          printf("Vehicle is \"%s\"\n", p->getBody()->getName());
          if (p->getControlledBy() == pEvt->getClient()) {
            printf("OK, vehicle occupied by correct client ... abandon!\n");

            CLeaveVehicle *pLeave = new CLeaveVehicle(1);
            pLeave->setNode(p->getBody()->getID());
            pLeave->setClient(p->getControlledBy());
            irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(pLeave);

            p->setControlledBy(-1);
            return false;
          }
        }
      }
    }
  }

  return true;
}

bool CVehicle::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == irr::ode::eIrrOdeEventStep || pEvent->getType() == eCtrlMsgRequestVehicle || pEvent->getType() == eCtrlMsgLeaveVehicle;
}
