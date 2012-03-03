#ifndef _C_IRR_ODE_AERO_DRAG
  #define _C_IRR_ODE_AERO_DRAG

  #include <motors/IIrrOdeStepMotor.h>

namespace irr {
  namespace ode {

const int IRR_ODE_AERO_DRAG_ID=MAKE_IRR_ID('i','o','a','d');
const wchar_t IRR_ODE_AERO_DRAG_NAME[0xFF]=L"CIrrOdeAeroDrag";
/**
 * @class CIrrOdeAeroDrag
 * This class "simulates" all forces that are applied to an aircraft during flight. It's not even close
 * to reality, but imho it's fun.
 */
class CIrrOdeAeroDrag : public IIrrOdeStepMotor {
  protected:
    irr::core::vector3df m_vForeward,  /*!< the foreward vector */
              m_vSideward,  /*!< the sideward vector */
              m_vUpward;    /*!< the upward vector */

    f32 m_fStallSpeed,      /*!< If the airplane is below this foreward velocity no up force is applied */
        m_fMaxUpSpeed,      /*!< This is the foreward speed where the highest up force is reached */
        m_fUpFactor,        /*!< uplift factor */
        m_fForewardDamp,    /*!< This is the foreward damping applied */
        m_fSidewardDamp,    /*!< The sideward damping applied */
        m_fUpwardDamp,      /*!< The damping applied from the angle of attack */
        m_fForewardVel,     /*!< This is the calculated foreward velocity. This value can't be set but queried */
        m_fPitchTrim,       /*!< pitch trim */
        m_fPitchTrimPwr,    /*!< pitch trim power */
        m_fUpToForeward,    /*!< factor for upward force pushing foreward */
        m_fUpDampToForeward,/*!< factor for upward damp force pushing foreward */
        m_fSideToForeward,  /*!< factor for sideward force pushing foreward */
        m_fDampMinVel,      /*!< the minimum velocity for sideward/upward damping */
        m_fDampMaxVel;      /*!< the velocity where the maximum side/up damping is reached */

  public:
    CIrrOdeAeroDrag(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                    const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
                    const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
                    const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));
    virtual void step();
    virtual const wchar_t *getTypeName();

    virtual irr::scene::ESCENE_NODE_TYPE getType() const { return (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_AERO_DRAG_ID; }

    void setForeward(irr::core::vector3df v) { m_vForeward=v; }
    void setSideward(irr::core::vector3df v) { m_vSideward=v; }
    void setUpward(irr::core::vector3df v) { m_vUpward=v; }

    irr::core::vector3df getForeward() { return m_vForeward; }
    irr::core::vector3df getSideward() { return m_vSideward; }
    irr::core::vector3df getUpward() { return m_vUpward; }

    void setStallSpeed(f32 f) { m_fStallSpeed=f; }
    void setMaxUpSpeed(f32 f) { m_fMaxUpSpeed=f; }
    void setForewardDamp(f32 f) { m_fForewardDamp=f; }
    void setSidewardDamp(f32 f) { m_fSidewardDamp=f; }
    void setUpwardDamp(f32 f) { m_fUpwardDamp=f; }
    void setUpFactor(f32 f) { m_fUpFactor=f; }

    void setPitchTrim(f32 f) { m_fPitchTrim=f; }
    void setPitchTrimPower(f32 f) { m_fPitchTrimPwr=f; }

    void setUpToForeward(f32 f) { m_fUpToForeward=f; }
    void setUpDampToForeward(f32 f) { m_fUpDampToForeward=f; }
    void setSideToForeward(f32 f) { m_fSideToForeward=f; }

    f32 getStallSpeed() { return m_fStallSpeed; }
    f32 getMaxUpSpeed() { return m_fMaxUpSpeed; }
    f32 getForewardDamp() { return m_fForewardDamp; }
    f32 getSidewardDamp() { return m_fSidewardDamp; }
    f32 getUpwardDamp() { return m_fUpwardDamp; }
    f32 getForewardVel() { return m_fForewardVel; }
    f32 getUpFactor() { return m_fUpFactor; }

    f32 getPitchTrim() { return m_fPitchTrim; }
    f32 getPitchTrimPower() { return m_fPitchTrimPwr; }

    f32 getUpToForeward() { return m_fUpToForeward; }
    f32 getUpDampToForeward() { return m_fUpDampToForeward; }
    f32 getSideToForeward() { return m_fSideToForeward; }

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);
    virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
};

  } //namespace ode
}   //namespace irr
#endif

