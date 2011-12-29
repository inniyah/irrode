  #include <tests/CTestCloneRemove.h>

CTestCloneRemove::CTestCloneRemove(IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Clone/Remove Test";
  m_sDescr=L"Test for the clone and remove functionality.";
}

void CTestCloneRemove::activate() {
  core::position2di cPos=core::position2di(10,10);
  core::dimension2du cDim=core::dimension2du(100,20);

  m_btnRemoveStatic =m_pGui->addButton(core::rect<s32>(cPos,cDim),NULL,-1,L"Remove Static" ); cPos.Y+=25;
  m_btnRemoveDynamic=m_pGui->addButton(core::rect<s32>(cPos,cDim),NULL,-1,L"Remove Dynamic"); cPos.Y+=50;
  m_btnClone        =m_pGui->addButton(core::rect<s32>(cPos,cDim),NULL,-1,L"Clone Object"  ); cPos.Y+=25;
  m_btnRemovecloned =m_pGui->addButton(core::rect<s32>(cPos,cDim),NULL,-1,L"Remove Cloned" ); cPos.Y+=25;
  m_pSmgr->loadScene("../../data/scenes/Testbed_cloneRemove.xml");

  m_pWorld   =reinterpret_cast<ode::CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("world"   ));
  m_pTemplate=reinterpret_cast<ode::CIrrOdeBody  *>(m_pSmgr->getSceneNodeFromName("template"));
  m_pStatic  =m_pSmgr->getSceneNodeFromName("staticRemove");
  m_pDynamic =m_pSmgr->getSceneNodeFromName("dynamicRemove");

  printf("scene nodes:\n\tworld=%i\n\ttemplate=%i\n\tstatic=%i\n\tdynamic=%i\n\n",(int)m_pWorld,(int)m_pTemplate,(int)m_pStatic,(int)m_pDynamic);

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNode();
  cam->setPosition(vector3df(-20,10,-20));
  cam->setTarget(vector3df(0,0,0));

  m_pRunner->setEventReceiver(this);
}

void CTestCloneRemove::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
  m_pGui->clear();
  m_pRunner->setEventReceiver(NULL);
}

s32 CTestCloneRemove::update() {
  s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}

bool CTestCloneRemove::OnEvent(const SEvent &event) {
  if (event.EventType==EET_GUI_EVENT) {
    if (event.GUIEvent.EventType==gui::EGET_BUTTON_CLICKED) {
      if (event.GUIEvent.Caller==m_btnRemoveDynamic) {
        if (m_pDynamic!=NULL) {
          ode::CIrrOdeBody *p=reinterpret_cast<ode::CIrrOdeBody *>(m_pDynamic);
          p->removeFromPhysics();
          m_pSmgr->addToDeletionQueue(p);
          m_pDynamic=NULL;
        }
      }

      if (event.GUIEvent.Caller==m_btnRemoveStatic ) {
        if (m_pStatic!=NULL) {
          core::list<scene::ISceneNode *> lChildren=m_pStatic->getChildren();
          core::list<scene::ISceneNode *>::Iterator it;

          for (it=lChildren.begin(); it!=lChildren.end(); it++) {
            scene::ISceneNode *pNode=*it;
            if (pNode->getType()==ode::IRR_ODE_GEOM_BOX_ID) {
              ode::CIrrOdeGeomBox *p=reinterpret_cast<ode::CIrrOdeGeomBox *>(pNode);
              p->removeFromPhysics();
            }
          }
          m_pSmgr->addToDeletionQueue(m_pStatic);
          m_pStatic=NULL;
        }
      }

      if (event.GUIEvent.Caller==m_btnClone        ) {
        ode::CIrrOdeBody *p=reinterpret_cast<ode::CIrrOdeBody *>(m_pTemplate->clone(m_pWorld));
        p->setPosition(core::vector3df(0.0f,20.0f,0.0f));
        p->initPhysics();
        m_lCloned.push_back(p);
      }

      if (event.GUIEvent.Caller==m_btnRemovecloned ) {
        if (m_lCloned.size()>0) {
          core::list<ode::CIrrOdeBody *>::Iterator it=m_lCloned.begin();
          ode::CIrrOdeBody *p=*(it);
          p->removeFromPhysics();
          m_pSmgr->addToDeletionQueue(p);
          m_lCloned.erase(it);
        }
      }
    }
  }
  return false;
}
