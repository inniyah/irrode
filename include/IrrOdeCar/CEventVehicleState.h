#ifndef _C_EVENT_RUDDER_POSITIONS
  #define _C_EVENT_RUDDER_POSITIONS


  #include <event/IIrrOdeEvent.h>
  #include <irrlicht.h>

#define EVENT_PLANE_STATE_ID irr::ode::eIrrOdeEventUser+1
#define EVENT_TANK_STATE_ID  irr::ode::eIrrOdeEventUser+2
#define EVENT_CAR_STATE_ID  irr::ode::eIrrOdeEventUser+3
#define EVENT_FIRE_SND_ID irr::ode::eIrrOdeEventUser+4
#define EVENT_HELI_STATE_ID irr::ode::eIrrOdeEventUser+5
#define EVENT_INST_FOREST_ID irr::ode::eIrrOdeEventUser+6
#define EVENT_LAP_TIME_ID irr::ode::eIrrOdeEventUser+7
#define EVENT_AUTOPILOT_ID irr::ode::eIrrOdeEventUser+8
#define EVENT_SELECT_TARGET_ID irr::ode::eIrrOdeEventUser+9
#define EVENT_SHOTS_ID irr::ode::eIrrOdeEventUser+10

class CEventPlaneState : public irr::ode::IIrrOdeEvent {
  protected:
    irr::s32 m_iNodeId;
    irr::s8  m_iYaw,
             m_iRoll,
             m_iPitch;
    irr::f32 m_fSound,
             m_fThrust;
    irr::u8  m_iFlags;

    static const irr::u8 s_iBrakes       = 1,
                         s_iAutoPilot    = 2;

  public:
    CEventPlaneState();
    CEventPlaneState(irr::s32 iId, irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll, irr::f32 fSound, bool bBrakes, bool bAp, irr::f32 fThrust);
    CEventPlaneState(irr::ode::CSerializer *pData);

    virtual ~CEventPlaneState() { }

    virtual irr::u16 getType() { return EVENT_PLANE_STATE_ID; }

    virtual irr::ode::CSerializer *serialize();
    virtual const irr::c8 *toString();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::f32 getYaw() { return ((irr::f32)m_iYaw)/100.0f; }
    irr::f32 getPitch() { return ((irr::f32)m_iPitch)/100.0f; }
    irr::f32 getRoll() { return ((irr::f32)m_iRoll)/100.0f; }
    irr::f32 getSound() { return m_fSound; }
    irr::f32 getThrust() { return m_fThrust; }
    irr::s32 getNodeId() { return m_iNodeId; }

    bool isBrakesOn    () { return m_iFlags & s_iBrakes      ; }
    bool isAutoPilotOn () { return m_iFlags & s_iAutoPilot   ; }
};

class CEventTankState : public irr::ode::IIrrOdeEvent {
  protected:
    irr::s32 m_iNodeId;
    irr::s8 m_aAngles[4];
    irr::f32 m_fCannonAngle,
             m_fTurretAngle,
             m_fSound;

  public:
    CEventTankState();
    CEventTankState(irr::s32 iId, const irr::s8 *aAngles, irr::f32 fCannonAngle, irr::f32 fTurretAngle, irr::f32 fSound);
    CEventTankState(irr::ode::CSerializer *pData);

    virtual ~CEventTankState() { }

    virtual irr::u16 getType() { return EVENT_TANK_STATE_ID; }

    virtual irr::ode::CSerializer *serialize();
    virtual const irr::c8 *toString();
    virtual irr::ode::IIrrOdeEvent *clone();

    const irr::s8 *getAngles() { return m_aAngles; }
    irr::s32 getNodeId() { return m_iNodeId; }
    irr::f32 getCannonAngle() { return m_fCannonAngle; }
    irr::f32 getTurretAngle() { return m_fTurretAngle; }
    irr::f32 getSound() { return m_fSound; }
};

class CEventCarState : public irr::ode::IIrrOdeEvent {
  public:
    enum eCarFlags {
      eCarFlagSmoke=1,
      eCarFlagBrake=2,
      eCarFlagReverse=4,
      eCarFlagDifferential=8,
      eCarFlagTouch = 16,
      eCarFlagBoost = 32,
      eCarFlagAdapt = 64
    };

  protected:
    irr::s32 m_iNodeId,
             m_iBoost;
    irr::f32 m_fSuspension,
             m_fLeftWheel,
             m_fRightWheel,
             m_fRpm,
             m_fDiff,
             m_fSound,
             m_fSteer,
             m_fSpeed,
             m_fWheelRot[2],
             m_fWheelPos[2];
    irr::u8  m_iFlags;
    irr::s8  m_iGear;

  public:
    CEventCarState();
    CEventCarState(irr::s32 iId, irr::f32 fSuspension, irr::f32 fLeftWheel, irr::f32 fRightWheel, irr::f32 fRpm, irr::f32 fDiff, irr::f32 fSound, irr::f32 fSteer, irr::u8 iFlags, irr::f32 fSpeed, irr::s8 iGear, irr::s32 iBoost, irr::f32 fWheelPos[2], irr::f32 fWheelRot[2]);
    CEventCarState(irr::ode::CSerializer *pData);

    virtual irr::u16 getType() { return EVENT_CAR_STATE_ID; }
    virtual const irr::c8 *toString();

    virtual irr::ode::CSerializer *serialize();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::s32 getNodeId() { return m_iNodeId; }
    irr::f32 getSuspension() { return m_fSuspension; }
    irr::f32 getLeftWheel() { return m_fLeftWheel; }
    irr::f32 getRightWheel() { return m_fRightWheel; }
    irr::f32 getRpm() { return m_fRpm; }
    irr::f32 getDiff() { return m_fDiff; }
    irr::f32 getEngineSound() { return m_fSound; }
    irr::f32 getSteer() { return m_fSteer; }
    irr::f32 getSpeed() { return m_fSpeed; }
    irr::u8 getFlags() { return m_iFlags; }
    irr::s8 getGear() { return m_iGear; }
    irr::s32 getBoost() { return m_iBoost; }

    irr::f32 getWheelPos(irr::u32 idx) { return idx < 2 ? m_fWheelPos[idx] : 0.0f; }
    irr::f32 getWheelRot(irr::u32 idx) { return idx < 2 ? m_fWheelRot[idx] : 0.0f; }
};

class CEventFireSound : public irr::ode::IIrrOdeEvent {
  public:
    enum enSound {
      eSndNone,
      eSndFireShell,
      eSndExplode,
      eSndCrash,
      eSndSkid,
      eSndBell,
      eSndShift,
      eSndCreaky
    };

  protected:
    irr::u8 m_iSound;
    irr::f32 m_fVolume;
    irr::core::vector3df m_vPos;

  public:
    CEventFireSound();
    CEventFireSound(enSound iSound, irr::f32 fVolume, irr::core::vector3df vPos);
    CEventFireSound(irr::ode::CSerializer *pData);

    virtual irr::u16 getType() { return EVENT_FIRE_SND_ID; }
    virtual const irr::c8 *toString();

    virtual irr::ode::CSerializer *serialize();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::u8 getSound() { return m_iSound; }
    irr::f32 getVolume() { return m_fVolume; }
    const irr::core::vector3df &getPosition() { return m_vPos; }
};

class CEventHeliState : public irr::ode::IIrrOdeEvent {
  protected:
    irr::s32 m_iNodeId;
    irr::f32 m_fSound,
             m_fThrust;
    bool m_bAutoPilot;

  public:
    CEventHeliState();
    CEventHeliState(irr::s32 iNodeId, irr::f32 fSound, bool bAutoPilot, irr::f32 fThrust);
    CEventHeliState(irr::ode::CSerializer *pData);

    virtual irr::u16 getType() { return EVENT_HELI_STATE_ID; }
    virtual const irr::c8 *toString();

    virtual irr::ode::CSerializer *serialize();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::f32 getSound() { return m_fSound; }
    irr::f32 getThrust() { return m_fThrust; }
    irr::s32 getNodeId() { return m_iNodeId; }
    bool isAutoPilotOn() { return m_bAutoPilot; }
};

class CEventInstallRandomForestPlugin : public irr::ode::IIrrOdeEvent {
  public:
    CEventInstallRandomForestPlugin() {
    }

    CEventInstallRandomForestPlugin(irr::ode::CSerializer *pData) {
      pData->resetBufferPos();
      irr::u16 iCode=pData->getU16();
      if (iCode!=EVENT_INST_FOREST_ID) printf("**** wrong message!\n");
    }

    virtual irr::u16 getType() { return EVENT_INST_FOREST_ID; }
    virtual const irr::c8 *toString() {
      strcpy(m_sString,"CEventInstallRandomForestPlugin");
      return m_sString;
    }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer==NULL) {
        m_pSerializer=new irr::ode::CSerializer();
        m_pSerializer->addU16(EVENT_INST_FOREST_ID);
      }
      return m_pSerializer;
    }

    virtual irr::ode::IIrrOdeEvent *clone() {
      return new CEventInstallRandomForestPlugin();
    }

    virtual bool isObservable() { return true; }
};

class CEventLapTime : public irr::ode::IIrrOdeEvent {
  protected:
    irr::f32 m_fTime;
    irr::s32 m_iBody,
             m_iCp;

  public:
    CEventLapTime(irr::f32 fTime, irr::s32 iBody, irr::s32 iCp) {
      m_fTime=fTime;
      m_iBody=iBody;
      m_iCp=iCp;
    }

    CEventLapTime(irr::ode::CSerializer *pData) {
      pData->resetBufferPos();
      irr::u16 iCode=pData->getU16();
      if (iCode==EVENT_LAP_TIME_ID) {
        m_fTime=pData->getF32();
        m_iBody=pData->getS32();
        m_iCp  =pData->getS16();
      }
    }

    virtual irr::u16 getType() { return EVENT_LAP_TIME_ID; }
    virtual const irr::c8 *toString() {
      strcpy(m_sString,"CEventLapTime");
      return m_sString;
    }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer==NULL) {
        m_pSerializer=new irr::ode::CSerializer();
        m_pSerializer->addU16(EVENT_LAP_TIME_ID);
        m_pSerializer->addF32(m_fTime);
        m_pSerializer->addS32(m_iBody);
        m_pSerializer->addS16(m_iCp);
      }
      return m_pSerializer;
    }

    virtual irr::ode::IIrrOdeEvent *clone() {
      return new CEventLapTime(m_fTime,m_iBody,m_iCp);
    }

    virtual bool isObservable() { return true; }

    irr::s32 getBodyId() { return m_iBody; }
    irr::s32 getCpId() { return m_iCp; }

    irr::f32 getTime() { return m_fTime; }
};

class CEventAutoPilot : public irr::ode::IIrrOdeEvent {
  private:
    bool m_bActive;
    irr::s32 m_iNextCp,
             m_iState,
             m_iObject;

  public:
    CEventAutoPilot(irr::s32 iObject, bool bActive, irr::s32 iNextCp, irr::s32 iState) {
      m_iObject = iObject;
      m_bActive = bActive;
      m_iNextCp = iNextCp;
      m_iState = iState;
    }

    CEventAutoPilot(irr::ode::CSerializer *pData) {
      pData->resetBufferPos();
      irr::u16 iCode = pData->getU16();
      if (iCode == EVENT_AUTOPILOT_ID) {
        m_iObject = pData->getS32();
        m_bActive = pData->getU8()!=0;
        m_iNextCp = pData->getS32();
        m_iState = pData->getS32();
      }
    }

    virtual irr::u16 getType() { return EVENT_AUTOPILOT_ID; }

    virtual const irr::c8 *toString() {
      strcpy(m_sString, "CEventAutoPilot");
      return m_sString;
    }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer = new irr::ode::CSerializer();
        m_pSerializer->addU16(EVENT_AUTOPILOT_ID);
        m_pSerializer->addS32(m_iObject);
        m_pSerializer->addU8(m_bActive ? 1 : 0);
        m_pSerializer->addS32(m_iNextCp);
        m_pSerializer->addS32(m_iState);
      }
      return m_pSerializer;
    }

    virtual irr::ode::IIrrOdeEvent *clone() {
      return new CEventAutoPilot(m_iObject, m_bActive, m_iNextCp, m_iState);
    }

    virtual bool isObservable() { return true; }

    irr::s32 getObject() { return m_iObject; }

    bool isActive() { return m_bActive; }

    irr::s32 getNextCp() { return m_iNextCp; }

    irr::s32 getState() { return m_iState; }
};

class CEventSelectTarget : public irr::ode::IIrrOdeEvent {
  private:
    irr::s32 m_iBodyId,
             m_iTargetId;

  public:
    CEventSelectTarget(irr::s32 iBodyId, irr::s32 iTargetId) {
      m_iBodyId = iBodyId;
      m_iTargetId = iTargetId;
    }

    CEventSelectTarget(irr::ode::CSerializer *pData) {
      pData->resetBufferPos();
      irr::u16 iCode = pData->getU16();
      if (iCode == EVENT_SELECT_TARGET_ID) {
        m_iBodyId = pData->getS32();
        m_iTargetId = pData->getS32();
      }
    }

    virtual irr::u16 getType() { return EVENT_SELECT_TARGET_ID; }

    virtual const irr::c8 *toString() {
      sprintf(m_sString, "CEventSelectTarget");
      return m_sString;
    }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer = new irr::ode::CSerializer();

        m_pSerializer->addU16(EVENT_SELECT_TARGET_ID);
        m_pSerializer->addS32(m_iBodyId);
        m_pSerializer->addS32(m_iTargetId);
      }
      return m_pSerializer;
    }

    virtual irr::ode::IIrrOdeEvent *clone() {
      return new CEventSelectTarget(m_iBodyId, m_iTargetId);
    }

    irr::s32 getBody() { return m_iBodyId; }

    irr::s32 getTarget() { return m_iTargetId; }
};

class CEventShots : public irr::ode::IIrrOdeEvent {
  private:
    irr::s32 m_iBodyId;
    irr::u32 m_iShotsFired,
             m_iHitsTaken,
             m_iHitsScored;

  public:
    CEventShots(irr::s32 iBodyId, irr::u32 iShotsFired, irr::u32 iHitsTaken, irr::u32 iHitsScored) {
      m_iBodyId     = iBodyId;
      m_iShotsFired = iShotsFired;
      m_iHitsTaken  = iHitsTaken;
      m_iHitsScored = iHitsScored;
    }

    CEventShots(irr::ode::CSerializer *pData) {
      pData->resetBufferPos();
      irr::u16 iCode = pData->getU16();
      if (iCode == EVENT_SHOTS_ID) {
        m_iBodyId     = pData->getS32();
        m_iShotsFired = pData->getU32();
        m_iHitsTaken  = pData->getU32();
        m_iHitsScored = pData->getU32();
      }
    }

    virtual irr::u16 getType() { return EVENT_SHOTS_ID; }

    virtual const irr::c8 *toString() {
      sprintf(m_sString,"CEventShots");
      return m_sString;
    }

    virtual irr::ode::CSerializer *serialize() {
      if (m_pSerializer == NULL) {
        m_pSerializer = new irr::ode::CSerializer();
        m_pSerializer->addU16(EVENT_SHOTS_ID);
        m_pSerializer->addS32(m_iBodyId);
        m_pSerializer->addU32(m_iShotsFired);
        m_pSerializer->addU32(m_iHitsTaken);
        m_pSerializer->addU32(m_iHitsScored);
      }
      return m_pSerializer;
    }

    virtual irr::ode::IIrrOdeEvent *clone() {
      return new CEventShots(m_iBodyId, m_iShotsFired, m_iHitsTaken, m_iHitsScored);
    }

    irr::s32 getBody() { return m_iBodyId; }

    irr::u32 getShotsFired() { return m_iShotsFired; }
    irr::u32 getHitsTaken () { return m_iHitsTaken ; }
    irr::u32 getHitsScored() { return m_iHitsScored; }
};

#endif
