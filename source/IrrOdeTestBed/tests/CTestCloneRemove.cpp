  #include <tests/CTestCloneRemove.h>

CTestCloneRemove::CTestCloneRemove(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Clone/Remove Test";
  m_sDescr=L"Test for the clone and remove functionality.";
}

void CTestCloneRemove::activate() {
  irr::core::position2di cPos=irr::core::position2di(10,10);
  irr::core::dimension2du cDim=irr::core::dimension2du(100,20);

  m_btnRemoveStatic =m_pGui->addButton(irr::core::rect<irr::s32>(cPos,cDim),NULL,-1,L"Remove Static" ); cPos.Y+=25;
  m_btnRemoveDynamic=m_pGui->addButton(irr::core::rect<irr::s32>(cPos,cDim),NULL,-1,L"Remove Dynamic"); cPos.Y+=50;
  m_btnClone        =m_pGui->addButton(irr::core::rect<irr::s32>(cPos,cDim),NULL,-1,L"Clone Object"  ); cPos.Y+=25;
  m_btnRemovecloned =m_pGui->addButton(irr::core::rect<irr::s32>(cPos,cDim),NULL,-1,L"Remove Cloned" ); cPos.Y+=25;
  m_pSmgr->loadScene("../../data/scenes/Testbed_cloneRemove.xml");

  m_pWorld   =reinterpret_cast<irr::ode::CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("world"   ));
  m_pTemplate=reinterpret_cast<irr::ode::CIrrOdeBody  *>(m_pSmgr->getSceneNodeFromName("template"));
  m_pStatic  =m_pSmgr->getSceneNodeFromName("staticRemove");
  m_pDynamic =m_pSmgr->getSceneNodeFromName("dynamicRemove");

  printf("scene nodes:\n\tworld=%i\n\ttemplate=%i\n\tstatic=%i\n\tdynamic=%i\n\n",(int)m_pWorld,(int)m_pTemplate,(int)m_pStatic,(int)m_pDynamic);

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  irr::scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNode();
  cam->setPosition(irr::core::vector3df(-20,10,-20));
  cam->setTarget(irr::core::vector3df(0,0,0));

  m_pRunner->setEventReceiver(this);
}

void CTestCloneRemove::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
  m_pGui->clear();
  m_pRunner->setEventReceiver(NULL);
}

irr::s32 CTestCloneRemove::update() {
  irr::s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}

bool CTestCloneRemove::OnEvent(const irr::SEvent &event) {
  if (event.EventType==irr::EET_GUI_EVENT) {
    if (event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED) {
      if (event.GUIEvent.Caller==m_btnRemoveDynamic) {
        if (m_pDynamic!=NULL) {
          irr::ode::CIrrOdeBody *p=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pDynamic);
          p->removeFromPhysics();
          m_pSmgr->addToDeletionQueue(p);
          m_pDynamic=NULL;
        }
      }

      if (event.GUIEvent.Caller==m_btnRemoveStatic ) {
        if (m_pStatic!=NULL) {
          irr::core::list<irr::scene::ISceneNode *> lChildren=m_pStatic->getChildren();
          irr::core::list<irr::scene::ISceneNode *>::Iterator it;

          for (it=lChildren.begin(); it!=lChildren.end(); it++) {
            irr::scene::ISceneNode *pNode=*it;
            if (pNode->getType()==irr::ode::IRR_ODE_GEOM_BOX_ID) {
              irr::ode::CIrrOdeGeomBox *p=reinterpret_cast<irr::ode::CIrrOdeGeomBox *>(pNode);
              p->removeFromPhysics();
            }
          }
          m_pSmgr->addToDeletionQueue(m_pStatic);
          m_pStatic=NULL;
        }
      }

      if (event.GUIEvent.Caller==m_btnClone) {
        irr::ode::CIrrOdeBody *p=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pTemplate->clone(m_pWorld));
        p->setPosition(irr::core::vector3df(0.0f,20.0f,0.0f));
        p->initPhysics();
        m_lCloned.push_back(p);
      }

      if (event.GUIEvent.Caller==m_btnRemovecloned ) {
        if (m_lCloned.size()>0) {
          irr::core::list<irr::ode::CIrrOdeBody *>::Iterator it=m_lCloned.begin();
          irr::ode::CIrrOdeBody *p=*(it);
          p->removeFromPhysics();
          m_pSmgr->addToDeletionQueue(p);
          m_lCloned.erase(it);
        }
      }
    }
  }
  return false;
}
