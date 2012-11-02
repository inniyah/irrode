  #include <IrrOdeRePLayerPlugin.h>

  #include <CAdvancedParticleSystemNode.h>

  #include <IrrOde.h>
  #include <observer/IEventFactory.h>
  #include <observer/CIrrOdeEventFactory.h>

  #include <CEventVehicleState.h>
  #include <SSharedManagers.h>
  #include <CCustomEventReceiver.h>

  #include <CRoadMeshLoader.h>
  #include <irrklang.h>
  #include <CMeshCombiner.h>
  #include <CRandomForestNode.h>

  #include <CCockpitCar.h>
  #include <CCockpitPlane.h>

  #include <CCameraController.h>

  #include <irrCC.h>

static CAdvancedParticleSystemNodeFactory *g_pFactory=NULL;
static CCameraController *g_pCamera=NULL;
static irr::IrrlichtDevice *g_pDevice = NULL;
static irrklang::ISoundEngine *g_pSnd = NULL;
static irr::ode::CIrrOdeManager *g_pOdeMngr = NULL;
static CIrrCC *g_pCtrl = NULL;
static irr::u32 g_iCtrls[16];
static irr::core::list<irr::ode::CIrrOdeBody *> g_lBodies;
static irr::core::list<irr::ode::CIrrOdeBody *>::Iterator g_itBody;

/**
 * We use the plugin to add an addition event listener to the IrrOdeRePlayer. This
 * listener must deactivate the particle systems of the missiles and shells so that
 * the replay looks like the original
 */
class APS_EventListener : public irr::ode::IIrrOdeEventListener {
  protected:
    irr::IrrlichtDevice *m_pDevice;
    irr::ode::CIrrOdeManager *m_pOdeMngr;
    irr::core::list<CCockpitPlane *> m_lPlaneCockpits;
    irr::core::list<CCockpitCar *> m_lCarCockpits;

  public:
    APS_EventListener(irr::IrrlichtDevice *pDevice, irr::ode::CIrrOdeManager *pOdeMngr) {
      m_pDevice=pDevice;
      m_pOdeMngr = pOdeMngr;
    }

    void searchPlaneBodies(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
        if (p->getOdeClassname().equals_ignore_case("plane")) {
          printf("\t\t#### add plane \"%s\"\n",pNode->getName());
          CCustomEventReceiver::getSharedInstance()->addPlane(p);
        }
      }
      irr::core::list<ISceneNode *> children=pNode->getChildren();
      irr::core::list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) searchPlaneBodies(*it);
    }

    void searchTankBodies(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
        if (p->getOdeClassname().equals_ignore_case("tank")) {
          printf("\t\t#### add tank \"%s\"\n",pNode->getName());
          CCustomEventReceiver::getSharedInstance()->addTank(p);
        }
      }
      irr::core::list<ISceneNode *> children=pNode->getChildren();
      irr::core::list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) searchTankBodies(*it);
    }

    void searchCarBodies(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
        if (p->getOdeClassname().equals_ignore_case("car")) {
          printf("\t\t#### add car \"%s\"\n",pNode->getName());
          CCustomEventReceiver::getSharedInstance()->addCar(p);
        }
      }
      irr::core::list<ISceneNode *> children=pNode->getChildren();
      irr::core::list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) searchCarBodies(*it);
    }

    void searchHeliBodies(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
        if (p->getOdeClassname().equals_ignore_case("heli")) {
          printf("\t\t#### add heli\"%s\"\n",pNode->getName());
          CCustomEventReceiver::getSharedInstance()->addHeli(p);
        }
      }
      irr::core::list<ISceneNode *> children=pNode->getChildren();
      irr::core::list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) searchHeliBodies(*it);
    }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent) {
      if (g_pCamera && g_pCamera->handlesEvent(pEvent)) g_pCamera->onEvent(pEvent);

      if (pEvent->getType()==irr::ode::eIrrOdeEventLoadScene) {
        searchPlaneBodies(m_pDevice->getSceneManager()->getRootSceneNode());
        searchTankBodies (m_pDevice->getSceneManager()->getRootSceneNode());
        searchCarBodies  (m_pDevice->getSceneManager()->getRootSceneNode());
        searchHeliBodies (m_pDevice->getSceneManager()->getRootSceneNode());

        const c8 sForests[][255]={ "RandomForest1", "RandomForest2", "Forest1", "Forest2" };

        for (u32 i=0; i<2; i++) {
          printf("merging \"%s\"...\n",sForests[i]);
          irr::scene::ISceneManager *smgr=m_pDevice->getSceneManager();
          scene::ISceneNode *p=smgr->getSceneNodeFromName(sForests[i]);
          CRandomForest *pForest=(CRandomForest *)p;
          if (pForest!=NULL) {
            CMeshCombiner *pCombine=new CMeshCombiner(0.8f);
            irr::core::array<irr::scene::IMeshSceneNode *> aMeshSceneNodes;
            irr::core::array<irr::scene::ISceneNode *> aTrees=pForest->getGeneratedTrees();

            for (u32 j=0; j<aTrees.size(); j++) {
              irr::scene::IMeshSceneNode *p=(irr::scene::IMeshSceneNode *)aTrees[j];
              aMeshSceneNodes.push_back(p);
            }

            printf("%i trees\n",aMeshSceneNodes.size());

            if (aMeshSceneNodes.size()>0) {
              c8 s[0xFF];
              sprintf(s,"MyCombinedTrees_%i",i);
              irr::scene::IMesh *pCombined=pCombine->combineMeshes(smgr,m_pDevice->getVideoDriver(),aMeshSceneNodes,s);
              if (pCombined!=NULL) {
                irr::scene::ISceneNode *pRoot=smgr->getSceneNodeFromName(sForests[i+2]);
                irr::scene::IMeshSceneNode *pNode=smgr->addMeshSceneNode(pCombined,pRoot==NULL?smgr->getRootSceneNode():pRoot);
                for (u32 i=0; i<pNode->getMaterialCount(); i++) {
                  pNode->getMaterial(i).setFlag(irr::video::EMF_LIGHTING,false);
                  pNode->getMaterial(i).MaterialType=irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
                }
              }
            }
          }
        }
        return true;
      }

      if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
        irr::core::list<CCockpitPlane *>::Iterator pit;
        for (pit = m_lPlaneCockpits.begin(); pit != m_lPlaneCockpits.end(); pit++) (*pit)->update();

        irr::core::list<CCockpitCar *>::Iterator cit;
        for (cit = m_lCarCockpits.begin(); cit != m_lCarCockpits.end(); cit++) (*cit)->update();

        if (g_pCamera) g_pCamera->update();
      }

      return false;
    }

    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
      return true;
    }
};

class APS_EventFactory : public irr::ode::IIrrOdeEventFactory {
  public:
    virtual irr::ode::IIrrOdeEvent *createMessage(irr::ode::CSerializer *pData, irr::scene::ISceneManager *pSmgr) {
      pData->resetBufferPos();
      irr::u16 iCode=pData->getU16();

      if (iCode==EVENT_PLANE_STATE_ID  ) return new CEventPlaneState  (pData);
      if (iCode==EVENT_TANK_STATE_ID   ) return new CEventTankState   (pData);
      if (iCode==EVENT_CAR_STATE_ID    ) return new CEventCarState    (pData);
      if (iCode==EVENT_HELI_STATE_ID   ) return new CEventHeliState   (pData);
      if (iCode==EVENT_FIRE_SND_ID     ) return new CEventFireSound   (pData);
      if (iCode==EVENT_AUTOPILOT_ID    ) return new CEventAutoPilot   (pData);
      if (iCode==EVENT_LAP_TIME_ID     ) return new CEventLapTime     (pData);
      if (iCode==EVENT_AUTOPILOT_ID    ) return new CEventAutoPilot   (pData);
      if (iCode==EVENT_SELECT_TARGET_ID) return new CEventSelectTarget(pData);
      if (iCode==EVENT_SHOTS_ID        ) return new CEventShots       (pData);

      if (iCode==EVENT_INST_FOREST_ID) return new CEventInstallRandomForestPlugin(pData);

      return NULL;
    }
};

static APS_EventListener *g_pListener=NULL;
static APS_EventFactory *g_pEventFactory=NULL;

int DLL_EXPORT install(irr::IrrlichtDevice *pDevice, void *pUserData) {
  SSharedManagers *pManagers=(SSharedManagers *)pUserData;

  g_pDevice = pDevice;
  g_pOdeMngr = ((irr::ode::CIrrOdeManager *)pManagers->m_pOdeManager);
  irr::ode::CIrrOdeEventFactory *pFac=((irr::ode::CIrrOdeEventFactory *)pManagers->m_pEventFactory);
  g_pSnd = ((irrklang::ISoundEngine *)pManagers->m_pSndEngine);

  if (g_pFactory==NULL) {
    printf("registering scenenode factory...\n");
    g_pFactory=new CAdvancedParticleSystemNodeFactory(pDevice->getSceneManager());
    pDevice->getSceneManager()->registerSceneNodeFactory(g_pFactory);
  }

  printf("registering event listener...\n");
  g_pListener=new APS_EventListener(pDevice,g_pOdeMngr);
  g_pOdeMngr->getIrrThread()->getInputQueue()->addEventListener(g_pListener);

  printf("registering event factory...\n");
  g_pEventFactory=new APS_EventFactory();
  pFac->registerEventFactory(g_pEventFactory);

  CRoadMeshLoader *pLoader=new CRoadMeshLoader(pDevice);
  pDevice->getSceneManager()->addExternalMeshLoader(pLoader);

  CCustomEventReceiver::setMembers(g_pDevice,g_pOdeMngr,g_pSnd);
  CCustomEventReceiver::getSharedInstance()->install();

  return 0;
}

int DLL_EXPORT destall(irr::IrrlichtDevice *pDevice, void *pUserData) {
  SSharedManagers *pManagers=(SSharedManagers *)pUserData;

  ((irr::ode::CIrrOdeManager *)pManagers->m_pOdeManager)->getIrrThread()->getInputQueue()->removeEventListener(g_pListener);
  delete g_pListener;
  g_pListener=NULL;

  ((irr::ode::CIrrOdeEventFactory *)pManagers->m_pEventFactory)->removeEventFactory(g_pEventFactory);
  delete g_pEventFactory;
  g_pEventFactory=NULL;

  CCustomEventReceiver::getSharedInstance()->destall();

  if (g_pCtrl) delete g_pCtrl;

  return 0;
}

int DLL_EXPORT handleCamera() {
  printf("Handle Camera!");
  g_pCtrl = new CIrrCC(g_pDevice);

  g_iCtrls[eCameraLeft    ] = g_pCtrl->addItem(3, stringw("Camera Left"    ), KEY_KEY_Y, CIrrCC::eCtrlAxis);
  g_iCtrls[eCameraRight   ] = g_pCtrl->addItem(3, stringw("Camera Right"   ), KEY_KEY_C, CIrrCC::eCtrlAxis);
  g_iCtrls[eCameraUp      ] = g_pCtrl->addItem(3, stringw("Camera Up"      ), KEY_KEY_F, CIrrCC::eCtrlAxis);
  g_iCtrls[eCameraDown    ] = g_pCtrl->addItem(3, stringw("Camera Down"    ), KEY_KEY_V, CIrrCC::eCtrlAxis);
  g_iCtrls[eCameraCenter  ] = g_pCtrl->addItem(3, stringw("Center Camera"  ), KEY_KEY_X, CIrrCC::eCtrlButton);
  g_iCtrls[eCameraInternal] = g_pCtrl->addItem(3, stringw("Toggle Internal"), KEY_KEY_I, CIrrCC::eCtrlToggleButton);

  g_pCtrl->createAxis(g_iCtrls[eCameraLeft], g_iCtrls[eCameraRight]);
  g_pCtrl->createAxis(g_iCtrls[eCameraUp  ], g_iCtrls[eCameraDown ]);

  g_pCamera = new CCameraController(g_pDevice, g_pSnd, g_pCtrl, g_pOdeMngr, 1.0f);
  g_pCamera->setCtrl(g_iCtrls);
  return 0;
}

bool DLL_EXPORT handleEvent(const irr::SEvent &event) {
  bool bRet = false;
  if (g_pCtrl) g_pCtrl->OnEvent(event);
  if (g_pCamera) bRet = g_pCamera->OnEvent(event);

  if (!bRet) {
    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
      if (event.KeyInput.PressedDown && event.KeyInput.Key == irr::KEY_TAB) {
        if (g_itBody == g_lBodies.end()) {
          g_pCamera->setTarget(NULL);
          g_itBody = g_lBodies.begin();
        }
        else {
          g_pCamera->setTarget(*g_itBody);
          g_itBody++;
        }
      }
    }
  }

  return bRet;
}

void DLL_EXPORT physicsInitialized(void) {
  printf("physicsInitialized!\n");
  irr::scene::ISceneManager *pSmgr = g_pDevice->getSceneManager();

  char sName[][0xFF] = { "car%i", "plane%i", "heli%i", "tank%i" };
  bool b = true;
  int i = 1;

  do {
    char s[0xFF];
    b = false;

    for (int j = 0; j < 4; j++) {
      sprintf(s,sName[j],i);
      irr::ode::CIrrOdeBody *p = reinterpret_cast<irr::ode::CIrrOdeBody *>(pSmgr->getSceneNodeFromName(s));
      if (p && p->isVisible()) {
        printf("\t\t\"%s\"\n",s);
        g_lBodies.push_back(p);
        b = true;
      }
    }
    i++;
  }
  while (b);

  g_itBody = g_lBodies.end();
}
