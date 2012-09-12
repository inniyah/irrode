  #include <CVehicle.h>

  #include <CPlane.h>
  #include <CTank.h>
  #include <CHeli.h>
  #include <CCar.h>

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

  m_pSmgr = pDevice->getSceneManager();

  fillBodyList(m_lCars  ,m_pSmgr->getRootSceneNode(),"car"  ,iNumCars  , m_pWorld);
  fillBodyList(m_lPlanes,m_pSmgr->getRootSceneNode(),"plane",iNumPlanes, m_pWorld);
  fillBodyList(m_lTanks ,m_pSmgr->getRootSceneNode(),"tank" ,iNumTanks , m_pWorld);
  fillBodyList(m_lHelis ,m_pSmgr->getRootSceneNode(),"heli" ,iNumHelis , m_pWorld);

  printf("\nvehicles found:\n\ncars: %i (%i)\nplanes: %i (%i)\nhelicopters: %i (%i)\ntanks: %i (%i)\n\n",m_lCars.size(),iNumCars,m_lPlanes.size(),iNumPlanes,m_lHelis.size(),iNumHelis,m_lTanks.size(),iNumTanks);

  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  CRearView *pRearView=NULL;

  if (bRearView) pRearView=new CRearView(m_pDevice,"rearview.jpg",m_pSmgr->addCameraSceneNode());

  for (it=m_lPlanes.begin(); it!=m_lPlanes.end(); it++) {
    CPlane *p=new CPlane(m_pDevice,*it,NULL,pRearView, pInputQueue);
    CCockpitPlane *pCockpit=new CCockpitPlane(m_pDevice,"instruments",p->getBody());
    p->setCockpit(pCockpit);
    m_lCockpits.push_back(pCockpit);
    m_lVehicles.push_back(p);
  }

  for (it=m_lCars.begin(); it!=m_lCars.end(); it++) {
    CCar *p=new CCar(m_pDevice,*it,pRearView, pInputQueue);
    CCockpitCar *pCarCockpit=new CCockpitCar(m_pDevice,"z_instru.jpg",p->getBody());
    p->setCockpit(pCarCockpit);
    m_lCockpits.push_back(pCarCockpit);
    m_lVehicles.push_back(p);
  }

  for (it=m_lTanks.begin(); it!=m_lTanks.end(); it++) {
    CTank *p=new CTank(m_pDevice,*it, pInputQueue);
    m_lVehicles.push_back(p);
  }

  for (it=m_lHelis.begin(); it!=m_lHelis.end(); it++) {
    CHeli *p=new CHeli(m_pDevice,*it,pRearView, pInputQueue);
    CCockpitPlane *pCockpit=new CCockpitPlane(m_pDevice,"instruments",p->getBody());
    p->setCockpit(pCockpit);
    m_lCockpits.push_back(pCockpit);
    m_lVehicles.push_back(p);
  }

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
}

CVehicle::~CVehicle() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

bool CVehicle::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType() == irr::ode::eIrrOdeEventStep) {
    irr::core::list<IRenderToTexture *>::Iterator it;
    for (it = m_lCockpits.begin(); it!=m_lCockpits.end(); it++) (*it)->update();
  }

  return true;
}

bool CVehicle::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == irr::ode::eIrrOdeEventStep;
}
