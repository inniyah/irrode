#ifndef _IRR_ODE_JOINT
  #define _IRR_ODE_JOINT

  #include <event/IIrrOdeEventWriter.h>
  #include <CIrrOdeSceneNode.h>
  #include <CIrrOdeBody.h>

namespace irr {
namespace ode {

const wchar_t IRR_ODE_PARAM_NAMES[12][0xFF]= {
  L"HiStop", L"LoStop", L"Velocity", L"Max. Force", L"Fudge Factor", L"Bounce", L"CFM",
  L"Stop ERP", L"Stop CFM", L"Suspension ERP", L"Suspension CFM"
};

class CIrrOdeJoint : public CIrrOdeSceneNode, public IIrrOdeEventWriter {
  public:
    enum eJointParameter {
      eHiStop,
      eLoStop,
      eVel,
      eMaxForce,
      eFudge,
      eBounce,
      eCFM,
      eStopERP,
      eStopCFM,
      eSuspensionERP,
      eSuspensionCFM,
      eParamEnd
    };

  protected:
    u32 m_iJointId;

    CIrrOdeBody *m_pBody1,
                *m_pBody2;

    CIrrOdeWorld *m_pWorld;  /**< the IrrOde world the joint is in */

    f32 m_fParam[eParamEnd][3];
    bool m_bParamUsed[eParamEnd][3],
         m_bUpdateVariables,
         m_bSerializeEvents;

  public:
    CIrrOdeJoint(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                 const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		             const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		             const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeJoint();

    void setBody1(CIrrOdeBody *pBody1);
    void setBody2(CIrrOdeBody *pBody2);

    CIrrOdeBody *getBody1();
    CIrrOdeBody *getBody2();

    void initLinkedObjects();

    virtual u16 numParamGroups() const =0;

    virtual f32 getParameter(u16 iGroup, eJointParameter iParam);
    virtual void setParameter(u16 iGroup, eJointParameter iParam, f32 fValue);
    virtual void setPosition(const irr::core::vector3df &newPos);

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

    virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);
    virtual void removeFromPhysics();

    void activateBodies();

    virtual void initPhysics();

    /**
     * This method is called when an event is posted
     * @param pEvent the posted event
     */
    virtual bool onEvent(IIrrOdeEvent *pEvent) { return false; }

    /**
     * This method is called to see whether or not an event is handled by this irr::core::listener
     * @param pEvent the event in question
     * @return "true" if the irr::core::listener handles the event, "false" otherwise
     */
    virtual bool handlesEvent(IIrrOdeEvent *pEvent) { return false; }

    /**
     * The implementation of the "writeEvent" method of the "IIrrOdeEventWriter" interface.
     * @return NULL, because the ODE object's events will be written by the IrrOdeDevice.
     * @see IIrrOdeEventWriter::writeEvent
     */
    virtual IIrrOdeEvent *writeEvent() { return NULL; }

    /**
     * This implementation shows that we are unknown to the IrrOdeDevice's
     * event creation method. Has to be overridden by all joints that are
     * handled by the IrrOdeDevice.
     */
    virtual eEventWriterType getEventWriterType() {
      return eIrrOdeEventWriterUnknown;
    }

    u32 getJointId() { return m_iJointId; }

    bool doesUpdateVariables() { return m_bUpdateVariables; }
    bool doesSerializeEvents() { return m_bSerializeEvents; }
};

} //namespace ode
} //namespace irr

#endif
