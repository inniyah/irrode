#ifndef _C_CONTROL_EVENTS
  #define _C_CONTROL_EVENTS

  #include <irrode.h>

enum enControlMessageIDs {
  eCtrlMsgLogin = irr::ode::eIrrOdeEventUser + 1024,
  eCtrlMsgLoginOk,
  eCtrlMsgRequestVehicle,
  eCtrlMsgLeaveVehicle,
  eCtrlMsgVehicleApproved,
  eCtrlMsgCar,
  eCtrlMsgPlane,
  eCtrlMsgTank
};

enum eCarCtrl {
  eCarForeward,
  eCarBackward,
  eCarLeft,
  eCarRight,
  eCarFlip,
  eCarDifferential,
  eCarShiftUp,
  eCarShiftDown,
  eCarBoost,
  eCarAdapSteer
};

enum eAerealControls {
  eAeroPitchUp,
  eAeroPitchDown,
  eAeroRollLeft,
  eAeroRollRight,
  eAeroYawLeft,
  eAeroYawRight,
  eAeroPowerUp,
  eAeroPowerDown,
  eAeroPowerZero,
  eAeroBrake,
  eAeroFirePrimary,
  eAeroFireSecondary,
  eAeroSelectTarget,
  eAeroAutoPilot,
  eAeroFlip
};

enum eTankCtrl {
  eTankForeward,
  eTankBackward,
  eTankLeft,
  eTankRight,
  eTankCannonLeft,
  eTankCannonRight,
  eTankCannonUp,
  eTankCannonDown,
  eTankFire,
  eTankFlip,
  eTankFastCollision
};

class IControlMessage : public irr::ode::IIrrOdeEvent {
  protected:
    irr::u16 m_iClient;
    irr::s32 m_iNode;

  public:
    IControlMessage() {
      m_iClient = 0;
      m_iNode = 0;
    }

    virtual ~IControlMessage() {
      m_iClient = 0;
    }

    void setClient(irr::u16 iClient) { m_iClient = iClient; }
    void setNode  (irr::s32 iNode  ) { m_iNode   = iNode  ; }

    irr::s32 getNode() { return m_iNode; }

    irr::u16 getClient() { return m_iClient; }
};

class CLoginMessage : public irr::ode::IIrrOdeEvent {
  public:
    CLoginMessage() { }

    CLoginMessage(irr::ode::CSerializer *p) { }

    virtual ~CLoginMessage() { }

    virtual irr::u16 getType() { return eCtrlMsgLogin; }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer = new irr::ode::CSerializer();
        m_pSerializer->addU16(eCtrlMsgLogin);
      }
      return m_pSerializer;
    }

    virtual irr::ode::IIrrOdeEvent *clone() { return new CLoginMessage(); }

    virtual const irr::c8 *toString() { sprintf(m_sString, "CLoginMessage"); return m_sString; }
};

class CLeaveVehicle : public IControlMessage {
  private:
    irr::u8 m_iAnswer;    /**!< server sets this to "1" to indicate that a client has left a vehicle */

  public:
    CLeaveVehicle(irr::u8 iAnswer) : IControlMessage() {
      m_iAnswer = iAnswer;
    }

    CLeaveVehicle(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType() == eCtrlMsgLeaveVehicle) {
        CLeaveVehicle *p = reinterpret_cast<CLeaveVehicle *>(pEvent);
        m_iNode   = p->getNode  ();
        m_iClient = p->getClient();
        m_iAnswer = p->getAnswer();
      }
    }

    CLeaveVehicle(irr::ode::CSerializer *p) {
      p->resetBufferPos();
      irr::u16 iType = p->getU16();
      if (iType == eCtrlMsgLeaveVehicle) {
        m_iNode   = p->getS32();
        m_iClient = p->getU16();
        m_iAnswer = p->getU8 ();
      }
    }

    virtual irr::u16 getType() { return eCtrlMsgLeaveVehicle; }

    virtual irr::ode::IIrrOdeEvent *clone() { return new CLeaveVehicle(this); }

    virtual const irr::c8 *toString() { sprintf(m_sString, "CLeaveVehicle: %i leaves %i.", m_iClient, m_iNode); return m_sString; }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer = new irr::ode::CSerializer();
        m_pSerializer->addU16(eCtrlMsgLeaveVehicle);
        m_pSerializer->addS32(m_iNode);
        m_pSerializer->addU16(m_iClient);
        m_pSerializer->addU8 (m_iAnswer);
      }
      return m_pSerializer;
    }

    irr::u8 getAnswer() { return m_iAnswer; }
};

class CVehicleApproved : public IControlMessage {
  public:
    CVehicleApproved() : IControlMessage() { }

    CVehicleApproved(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType() == eCtrlMsgVehicleApproved) {
        CVehicleApproved *p = reinterpret_cast<CVehicleApproved *>(pEvent);
        m_iNode   = p->getNode  ();
        m_iClient = p->getClient();
      }
    }

    CVehicleApproved(irr::ode::CSerializer *p) {
      p->resetBufferPos();
      irr::u16 iType = p->getU16();
      if (iType == eCtrlMsgVehicleApproved) {
        m_iNode   = p->getS32();
        m_iClient = p->getU16();
      }
    }

    virtual irr::u16 getType() { return eCtrlMsgVehicleApproved; }

    virtual irr::ode::IIrrOdeEvent *clone() { return new CVehicleApproved(this); }

    virtual const irr::c8 *toString() { sprintf(m_sString, "CVehicleApproved: client %i, vehicle %i\n", m_iClient, m_iNode); return m_sString; }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer->addU16(eCtrlMsgVehicleApproved);
        m_pSerializer->addS32(m_iNode  );
        m_pSerializer->addU16(m_iClient);
      }
      return m_pSerializer;
    }
};

class CRequestVehicle : public IControlMessage {
  public:
    CRequestVehicle() : IControlMessage() {
    }

    CRequestVehicle(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType() == eCtrlMsgRequestVehicle) {
        CRequestVehicle *p = reinterpret_cast<CRequestVehicle *>(pEvent);
        m_iClient = p->getClient();
        m_iNode   = p->getNode();
      }
    }

    CRequestVehicle(irr::ode::CSerializer *p) {
      p->resetBufferPos();
      irr::u16 iType = p->getU16();
      if (iType == eCtrlMsgRequestVehicle) {
        m_iNode = p->getS32();
      }
    }

    virtual ~CRequestVehicle() { }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer = new irr::ode::CSerializer();
        m_pSerializer->addU16(eCtrlMsgRequestVehicle);
        m_pSerializer->addS32(m_iNode);
      }
      return m_pSerializer;
    }

    virtual irr::u16 getType() { return eCtrlMsgRequestVehicle; }

    virtual irr::ode::IIrrOdeEvent *clone() { return new CRequestVehicle(this); }

    virtual const irr::c8 *toString() { sprintf(m_sString, "CRequestVehicle: %i", m_iNode); return m_sString; }
};

class CCarControls : public IControlMessage {
  public:
    enum eCarControlFlags {
      eCarControlFlip = 1,
      eCarControlDiff = 2,
      eCarControlShiftUp = 4,
      eCarControlShiftDown = 8,
      eCarControlBoost = 16,
      eCarControlAdaptiveSteer = 32
    };

  protected:
    irr::f32 m_fThrottle,
             m_fSteer;
    irr::u8  m_iFlags;

    void setFlag(bool bValue, irr::u8 iFlag) {
      if (bValue) m_iFlags = m_iFlags | iFlag; else m_iFlags = m_iFlags & ~iFlag;
    }

  public:
    CCarControls(irr::f32 fThrottle, irr::f32 fSteer) {
      m_fThrottle = fThrottle;
      m_fSteer = fSteer;
      m_iFlags = 0;
    }

    CCarControls() {
      m_fThrottle = 0.0f;
      m_fSteer = 0.0f;
      m_iFlags = 0;
    }

    CCarControls(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType() == eCtrlMsgCar) {
        CCarControls *p = reinterpret_cast<CCarControls *>(pEvent);
        m_fThrottle = p->getThrottle();
        m_fSteer    = p->getSteer   ();
        m_iFlags    = p->getFlags   ();
        m_iClient   = p->getClient  ();
        m_iNode     = p->getNode    ();
      }
    }

    CCarControls(irr::ode::CSerializer *p) {
      p->resetBufferPos();
      irr::u16 iType = p->getU16();
      if (iType == eCtrlMsgCar) {
        m_iNode     = p->getS32();
        m_fThrottle = p->getF32();
        m_fSteer    = p->getF32();
        m_iFlags    = p->getU8 ();
      }
    }

    virtual ~CCarControls() { }

    void setFlip         (bool b) { setFlag(b, eCarControlFlip         ); }
    void setDifferential (bool b) { setFlag(b, eCarControlDiff         ); }
    void setShiftUp      (bool b) { setFlag(b, eCarControlShiftUp      ); }
    void setShifDown     (bool b) { setFlag(b, eCarControlShiftDown    ); }
    void setBoost        (bool b) { setFlag(b, eCarControlBoost        ); }
    void setAdaptiveSteer(bool b) { setFlag(b, eCarControlAdaptiveSteer); }

    irr::f32 getThrottle() { return m_fThrottle; }
    irr::f32 getSteer   () { return m_fSteer   ; }

    bool getFlip         () { return m_iFlags & eCarControlFlip         ; }
    bool getDifferential () { return m_iFlags & eCarControlDiff         ; }
    bool getShiftUp      () { return m_iFlags & eCarControlShiftUp      ; }
    bool getShiftDown    () { return m_iFlags & eCarControlShiftDown    ; }
    bool getBoost        () { return m_iFlags & eCarControlBoost        ; }
    bool getAdaptiveSteer() { return m_iFlags & eCarControlAdaptiveSteer; }

    irr::u8 getFlags() { return m_iFlags; }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer = new irr::ode::CSerializer();
        m_pSerializer->addU16(eCtrlMsgCar);
        m_pSerializer->addS32(m_iNode);
        m_pSerializer->addF32(m_fThrottle);
        m_pSerializer->addF32(m_fSteer);
        m_pSerializer->addU8 (m_iFlags);
      }
      return m_pSerializer;
    }

    virtual irr::u16 getType() { return eCtrlMsgCar; }

    virtual irr::ode::IIrrOdeEvent *clone() { return new CCarControls(this); }

    virtual const irr::c8 *toString() { sprintf(m_sString, "CCarControls: %.2f, %.2f, %i", m_fThrottle, m_fSteer, m_iFlags); return m_sString; }
};

class CPlaneControls : public IControlMessage {
  public:
    enum ePlaneControlFlags {
      ePlaneControlPowerZero     = 1,
      ePlaneControlBrake         = 2,
      ePlaneControlSelectTarget  = 8,
      ePlaneControlFirePrimary   = 16,
      ePlaneControlFireSecondary = 32,
      ePlaneControlFlip          = 64,
      ePlaneControlAutoPilot     = 128
    };

  protected:
    irr::f32 m_fYaw,
             m_fPitch,
             m_fRoll,
             m_fPower;
    irr::u8  m_iFlags;

    void setFlag(bool bValue, irr::u16 iFlag) {
      if (bValue) m_iFlags = m_iFlags | iFlag; else m_iFlags = m_iFlags & ~iFlag;
    }

  public:
    CPlaneControls(irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll, irr::f32 fPower) {
      m_fYaw   = fYaw;
      m_fRoll  = fRoll;
      m_fPitch = fPitch;
      m_fPower = fPower;
      m_iFlags = 0;
    }

    CPlaneControls() {
      m_fYaw   = 0;
      m_fPitch = 0;
      m_fRoll  = 0;
      m_iFlags = 0;
    }

    CPlaneControls(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType() == eCtrlMsgPlane) {
        CPlaneControls *p = reinterpret_cast<CPlaneControls *>(pEvent);
        m_fYaw    = p->getYaw   ();
        m_fPitch  = p->getPitch ();
        m_fRoll   = p->getRoll  ();
        m_fPower  = p->getPower ();
        m_iFlags  = p->getFlags ();
        m_iNode   = p->getNode  ();
        m_iClient = p->getClient();
      }
    }

    CPlaneControls(irr::ode::CSerializer *p) {
      p->resetBufferPos();
      irr::u16 iType = p->getU16();
      if (iType == eCtrlMsgPlane) {
        m_iNode   = p->getS32();
        m_fYaw    = p->getF32();
        m_fPitch  = p->getF32();
        m_fRoll   = p->getF32();
        m_fPower  = p->getF32();
        m_iFlags  = p->getU8 ();
      }
    }

    virtual ~CPlaneControls() { }

    void setPowerZero    (bool b) { setFlag(b, ePlaneControlPowerZero    ); }
    void setSelectTarget (bool b) { setFlag(b, ePlaneControlSelectTarget ); }
    void setFirePrimary  (bool b) { setFlag(b, ePlaneControlFirePrimary  ); }
    void setFireSecondary(bool b) { setFlag(b, ePlaneControlFireSecondary); }
    void setFlip         (bool b) { setFlag(b, ePlaneControlFlip         ); }
    void setBrake        (bool b) { setFlag(b, ePlaneControlBrake        ); }
    void setAutoPilot    (bool b) { setFlag(b, ePlaneControlAutoPilot    ); }

    bool getPowerZero    () { return m_iFlags & ePlaneControlPowerZero    ; }
    bool getSelectTarget () { return m_iFlags & ePlaneControlSelectTarget ; }
    bool getFirePrimary  () { return m_iFlags & ePlaneControlFirePrimary  ; }
    bool getFireSecondary() { return m_iFlags & ePlaneControlFireSecondary; }
    bool getFlip         () { return m_iFlags & ePlaneControlFlip         ; }
    bool getBrake        () { return m_iFlags & ePlaneControlBrake        ; }
    bool getAutoPilot    () { return m_iFlags & ePlaneControlAutoPilot    ; }

    irr::f32 getPitch() { return m_fPitch; }
    irr::f32 getYaw  () { return m_fYaw  ; }
    irr::f32 getRoll () { return m_fRoll ; }
    irr::f32 getPower() { return m_fPower; }

    irr::u16 getFlags() { return m_iFlags; }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer = new irr::ode::CSerializer();
        m_pSerializer->addU16(eCtrlMsgPlane);
        m_pSerializer->addS32(m_iNode );
        m_pSerializer->addF32(m_fYaw  );
        m_pSerializer->addF32(m_fPitch);
        m_pSerializer->addF32(m_fRoll );
        m_pSerializer->addF32(m_fPower);
        m_pSerializer->addU8 (m_iFlags);
      }
      return m_pSerializer;
    }

    virtual irr::u16 getType() { return eCtrlMsgPlane; }

    virtual irr::ode::IIrrOdeEvent *clone() { return new CPlaneControls(this); }

    virtual const irr::c8 *toString() { sprintf(m_sString, "CPlaneControls: %.2f, %.2f, %.2f, %i", m_fYaw, m_fPitch, m_fRoll, m_iFlags); return m_sString; }
};

class CTankControls : public IControlMessage {
  public:
    enum eTankControlFlags {
      eTankControlFire          = 1,
      eTankControlFlip          = 2,
      eTankControlFastCollision = 4
    };

  protected:
    irr::f32 m_fThrottle,
             m_fSteer,
             m_fCannonLeft,
             m_fCannonUp;
    irr::u8 m_iFlags;

    void setFlag(bool bValue, irr::u8 iFlag) {
      if (bValue) m_iFlags = m_iFlags | iFlag; else m_iFlags = m_iFlags & ~iFlag;
    }

  public:
    CTankControls(irr::f32 fThrottle, irr::f32 fSteer, irr::f32 fCannonLeft, irr::f32 fCannonUp) {
      m_fThrottle   = fThrottle;
      m_fSteer      = fSteer;
      m_fCannonLeft = fCannonLeft;
      m_fCannonUp   = fCannonUp;
      m_iFlags      = 0;
    }

    CTankControls() {
      m_fThrottle   = 0;
      m_fSteer      = 0;
      m_fCannonLeft = 0;
      m_fCannonUp   = 0;
      m_iFlags      = 0;
    }

    CTankControls(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType() == eCtrlMsgTank) {
        CTankControls *p = reinterpret_cast<CTankControls *>(pEvent);
        m_fThrottle   = p->getThrottle  ();
        m_fSteer      = p->getSteer     ();
        m_fCannonLeft = p->getCannonLeft();
        m_fCannonUp   = p->getCannonUp  ();
        m_iFlags      = p->getFlags     ();
        m_iClient     = p->getClient    ();
        m_iNode       = p->getNode      ();
      }
    }

    CTankControls(irr::ode::CSerializer *p) {
      p->resetBufferPos();
      irr::u16 iType = p->getU16();
      if (iType == eCtrlMsgTank) {
        m_iNode       = p->getS32();
        m_fThrottle   = p->getF32();
        m_fSteer      = p->getF32();
        m_fCannonLeft = p->getF32();
        m_fCannonUp   = p->getF32();
        m_iFlags      = p->getU16();
      }
    }

    virtual ~CTankControls() { }

    void setFire         (bool b) { setFlag(b, eTankControlFire         ); }
    void setFlip         (bool b) { setFlag(b, eTankControlFlip         ); }
    void setFastCollision(bool b) { setFlag(b, eTankControlFastCollision); }

    bool getFire         () { return m_iFlags & eTankControlFire         ; }
    bool getFlip         () { return m_iFlags & eTankControlFlip         ; }
    bool getFastCollision() { return m_iFlags & eTankControlFastCollision; }

    irr::f32 getThrottle  () { return m_fThrottle  ; }
    irr::f32 getSteer     () { return m_fSteer     ; }
    irr::f32 getCannonLeft() { return m_fCannonLeft; }
    irr::f32 getCannonUp  () { return m_fCannonUp  ; }

    irr::u8 getFlags() { return m_iFlags; }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer = new irr::ode::CSerializer();
        m_pSerializer->addU16(eCtrlMsgTank);
        m_pSerializer->addS32(m_iNode);
        m_pSerializer->addF32(m_fThrottle);
        m_pSerializer->addF32(m_fSteer);
        m_pSerializer->addF32(m_fCannonLeft);
        m_pSerializer->addF32(m_fCannonUp);
        m_pSerializer->addU8 (m_iFlags);
      }
      return m_pSerializer;
    }

    virtual irr::u16 getType() { return eCtrlMsgTank; }

    virtual irr::ode::IIrrOdeEvent *clone() { return new CTankControls(this); }

    virtual const irr::c8 *toString() { sprintf(m_sString, "CTankControls: %.2f, %.2f, %i", m_fThrottle, m_fSteer, m_iFlags); return m_sString; }
};
#endif
