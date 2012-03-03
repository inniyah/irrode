#ifndef _C_IRR_ODE_IMPULSE_MOTOR
  #define _C_IRR_ODE_IMPULSE_MOTOR

  #include <motors/IIrrOdeStepMotor.h>

namespace irr {
  namespace ode {

const int IRR_ODE_IMPULSE_MOTOR_ID=MAKE_IRR_ID('i','o','m','i');
const wchar_t IRR_ODE_IMPULSE_MOTOR_NAME[0xFF]=L"CIrrOdeImpulseMotor";
/**
 * @class CIrrOdeImpulseMotor
 * This class adds a certain force to the parent body at each step. Must be direct child of the affected body.
 * Is useful e.g. for planes or missiles.
 */
class CIrrOdeImpulseMotor : public IIrrOdeStepMotor {
  protected:
    irr::core::vector3df m_vForeward;

  public:
    CIrrOdeImpulseMotor(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                        const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
                        const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
                        const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));
    virtual void step();
    virtual const wchar_t *getTypeName();

    virtual irr::scene::ESCENE_NODE_TYPE getType() const { return (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_IMPULSE_MOTOR_ID; }

    void setForeward(irr::core::vector3df v) { m_vForeward=v; }
    irr::core::vector3df getForeward() { return m_vForeward; }

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);
    virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
};

  } //namespace ode
}   //namespace irr
#endif
