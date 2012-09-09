#ifndef _C_VEHICLE
  #define _C_VEHICLE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <CIrrOdeCarState.h>

class IRenderToTexture;
class CIrrCC;

class CVehicle : public irr::ode::IIrrOdeEventListener {
  private:
    irr::IrrlichtDevice *m_pDevice;
    irr::scene::ISceneManager *m_pSmgr;
    irr::ode::CIrrOdeWorld *m_pWorld;

    irr::core::list<irr::scene::ISceneNode *> m_lCars,
                                              m_lPlanes,
                                              m_lHelis,
                                              m_lTanks;
    irr::core::list<CIrrOdeCarState        *> m_lVehicles;
    irr::core::list<IRenderToTexture       *> m_lCockpits;

    CIrrCC *m_pController;

    void removeFromScene(irr::scene::ISceneNode *pNode, irr::ode::CIrrOdeWorld *pWorld);
    void fillBodyList(irr::core::list<irr::scene::ISceneNode *> &aVehicles, irr::scene::ISceneNode *pNode, const irr::c8 *sClassName, irr::u32 iMax, irr::ode::CIrrOdeWorld *pWorld);

  public:
    CVehicle(irr::IrrlichtDevice *pDevice, irr::u32 iNumCars, irr::u32 iNumPlanes, irr::u32 iNumHelis, irr::u32 iNumTanks, irr::ode::CIrrOdeWorld *pWorld, CIrrCC *pCtrl, bool bRearView, irr::u32 iCtrls[4][32]);
    ~CVehicle();

    const irr::core::list<CIrrOdeCarState *>getVehicles() {
      return m_lVehicles;
    }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif
