#ifndef _C_PLANE
  #define _C_PLANE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"
  #include <CEventVehicleState.h>
  #include <CAeroVehicle.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CTargetSelector;
class CCockpitPlane;
class CProjectile;
class CAutoPilot;

class CPlane : public CAeroVehicle {
  protected:
    ISceneNode *m_pRoll[2],
               *m_pPitch[2],
               *m_pYaw[2];

    bool m_bLeftMissile;
    ode::CIrrOdeJointHinge *m_pAxes[2];
    ode::CIrrOdeJointHinge2 *m_pSteerAxis;
    f32 m_fAngleRate[3];

  public:
    CPlane(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, irrklang::ISoundEngine *pSndEngine);
    virtual ~CPlane();

    virtual void odeStep(u32 iStep);

    virtual u32 update();

    virtual stringc &getButtonText() { static stringc s=stringc("select airplane"); return s; }
    virtual void drawSpecifics();
};

#endif


