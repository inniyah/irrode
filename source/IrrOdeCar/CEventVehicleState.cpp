  #include <CEventVehicleState.h>

CEventPlaneState::CEventPlaneState() {
  m_pSerializer=NULL;
  m_iNodeId=0;
  m_iYaw=0;
  m_iPitch=0;
  m_iRoll=0;
  m_iFlags = 0;
  m_fThrust = 0.0f;
}

CEventPlaneState::CEventPlaneState(irr::s32 iId, irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll, irr::f32 fSound, bool bBrakes, bool bAp, irr::f32 fThrust) {
  m_iNodeId=iId;
  m_pSerializer=NULL;
  m_iYaw=(irr::u8)(fYaw*100.0f);
  m_iPitch=(irr::u8)(fPitch*100.0f);
  m_iRoll=(irr::u8)(fRoll*100.0f);
  m_iFlags = 0;
  m_fThrust = fThrust;

  if (bBrakes      ) m_iFlags += s_iBrakes      ;
  if (bAp          ) m_iFlags += s_iAutoPilot   ;

  m_fSound=fSound;
}

CEventPlaneState::CEventPlaneState(irr::ode::CSerializer *pData) {
  pData->resetBufferPos();
  irr::u16 iCode=pData->getU16();
  if (iCode==EVENT_PLANE_STATE_ID) {
    m_iNodeId=pData->getS32();
    m_iYaw=pData->getS8();
    m_iPitch=pData->getS8();
    m_iRoll=pData->getS8();
    m_iFlags=pData->getU8();
    m_fSound=pData->getF32();
    m_fThrust = pData->getF32();
  }
}

irr::ode::CSerializer *CEventPlaneState::serialize() {
  if (m_pSerializer==NULL) {
    m_pSerializer=new irr::ode::CSerializer();
    m_pSerializer->addU16(EVENT_PLANE_STATE_ID);
    m_pSerializer->addS32(m_iNodeId);
    m_pSerializer->addS8(m_iYaw);
    m_pSerializer->addS8(m_iPitch);
    m_pSerializer->addS8(m_iRoll);
    m_pSerializer->addU8(m_iFlags);
    m_pSerializer->addF32(m_fSound);
    m_pSerializer->addF32(m_fThrust);
  }
  return m_pSerializer;
}

const irr::c8 *CEventPlaneState::toString() {
  sprintf(m_sString,"CEventPlaneState (%i): yaw=%i, pitch=%i, roll=%i",m_iNodeId,m_iYaw,m_iPitch,m_iRoll);
  return m_sString;
}

irr::ode::IIrrOdeEvent *CEventPlaneState::clone() {
  return new CEventPlaneState(m_iNodeId,((irr::f32)m_iYaw)/100.0f,((irr::f32)m_iPitch)/100.0f,((irr::f32)m_iRoll)/100.0f,m_fSound,m_iFlags&s_iBrakes,m_iFlags&s_iAutoPilot,m_fThrust);
}

CEventTankState::CEventTankState() {
  m_iNodeId=0;
  m_fSound=0.0f;
  for (irr::u32 i=0; i<4; i++) m_aAngles[i]=0;
  m_fCannonAngle=0.0f;
}

CEventTankState::CEventTankState(irr::s32 iId, const irr::s8 *aAngles, irr::f32 fCannonAngle, irr::f32 fTurretAngle, irr::f32 fSound) {
  m_iNodeId=iId;
  for (irr::u32 i=0; i<4; i++) m_aAngles[i]=aAngles[i];
  m_fCannonAngle=fCannonAngle;
  m_fTurretAngle=fTurretAngle;
  m_fSound=fSound;
}

CEventTankState::CEventTankState(irr::ode::CSerializer *pData) {
  pData->resetBufferPos();
  irr::u16 iCode=pData->getU16();
  if (iCode==EVENT_TANK_STATE_ID) {
    m_iNodeId=pData->getS32();
    for (irr::u32 i=0; i<4; i++) m_aAngles[i]=pData->getS8();

    m_fCannonAngle=pData->getF32();
    m_fTurretAngle=pData->getF32();
    m_fSound=pData->getF32();
  }
}

irr::ode::CSerializer *CEventTankState::serialize() {
  if (m_pSerializer==NULL) {
    m_pSerializer=new irr::ode::CSerializer();
    m_pSerializer->addU16(EVENT_TANK_STATE_ID);
    m_pSerializer->addS32(m_iNodeId);
    for (irr::u32 i=0; i<4; i++) {
      m_pSerializer->addS8(m_aAngles[i]);
    }
    m_pSerializer->addF32(m_fCannonAngle);
    m_pSerializer->addF32(m_fTurretAngle);
    m_pSerializer->addF32(m_fSound);
  }
  return m_pSerializer;
}

const irr::c8 *CEventTankState::toString() {
  sprintf(m_sString,"CEventTankState: %i, %i, %i, %i, %i, %.2f, %.2f",m_iNodeId,m_aAngles[0],m_aAngles[1],m_aAngles[2],m_aAngles[3],m_fCannonAngle,m_fTurretAngle);
  return m_sString;
}

irr::ode::IIrrOdeEvent *CEventTankState::clone() {
  return new CEventTankState(m_iNodeId,m_aAngles,m_fCannonAngle,m_fTurretAngle,m_fSound);
}

CEventCarState::CEventCarState() {
  m_iNodeId=0;
  m_fSuspension=0.0f;
  m_fLeftWheel=0.0f;
  m_fRightWheel=0.0f;
  m_iFlags=0;
  m_fSteer=0.0f;
  m_fSound=0.0f;
  m_fSpeed=0.0f;
}

CEventCarState::CEventCarState(irr::s32 iId, irr::f32 fSuspension, irr::f32 fLeftWheel, irr::f32 fRightWheel, irr::f32 fRpm, irr::f32 fDiff, irr::f32 fSound, irr::f32 fSteer, irr::u8 iFlags, irr::f32 fSpeed) {
  m_iNodeId=iId;
  m_fSuspension=fSuspension;
  m_fLeftWheel=fLeftWheel;
  m_fRightWheel=fRightWheel;
  m_fRpm=fRpm;
  m_iFlags=iFlags;
  m_fSound=fSound;
  m_fDiff=fDiff;
  m_fSteer=fSteer;
  m_fSpeed = fSpeed;
}

CEventCarState::CEventCarState(irr::ode::CSerializer *pData) {
  pData->resetBufferPos();
  irr::u16 iCode=pData->getU16();
  if (iCode==EVENT_CAR_STATE_ID) {
    m_iNodeId=pData->getS32();
    m_fSuspension=pData->getF32();
    m_fLeftWheel=pData->getF32();
    m_fRightWheel=pData->getF32();
    m_fRpm=pData->getF32();
    m_fDiff=pData->getF32();
    m_fSound=pData->getF32();
    m_fSteer=pData->getF32();
    m_iFlags=pData->getU8();
    m_fSpeed=pData->getF32();
  }
}

irr::ode::CSerializer *CEventCarState::serialize() {
  if (m_pSerializer==NULL) {
    m_pSerializer=new irr::ode::CSerializer();
    m_pSerializer->addU16(EVENT_CAR_STATE_ID);
    m_pSerializer->addS32(m_iNodeId);
    m_pSerializer->addF32(m_fSuspension);
    m_pSerializer->addF32(m_fLeftWheel);
    m_pSerializer->addF32(m_fRightWheel);
    m_pSerializer->addF32(m_fRpm);
    m_pSerializer->addF32(m_fDiff);
    m_pSerializer->addF32(m_fSound);
    m_pSerializer->addF32(m_fSteer);
    m_pSerializer->addU8(m_iFlags);
    m_pSerializer->addF32(m_fSpeed);
  }
  return m_pSerializer;
}

irr::ode::IIrrOdeEvent *CEventCarState::clone() {
  return new CEventCarState(m_iNodeId,m_fSuspension,m_fLeftWheel,m_fRightWheel,m_iFlags,m_fDiff,m_fSound,m_fSteer,m_fRpm,m_fSpeed);
}

const irr::c8 *CEventCarState::toString() {
  sprintf(m_sString,"CEventCarState (%i): %.2f, %.2f, %.2f, %.2f, %i",m_iNodeId,m_fSuspension,m_fLeftWheel,m_fRightWheel,m_fRpm,m_iFlags);
  return m_sString;
}

CEventFireSound::CEventFireSound() {
  m_fVolume=0.0f;
  m_iSound=0;
  m_vPos=irr::core::vector3df(0.0f,0.0f,0.0f);
}

CEventFireSound::CEventFireSound(enSound iSound, irr::f32 fVolume, irr::core::vector3df vPos) {
  m_iSound=iSound;
  m_fVolume=fVolume;
  m_vPos=vPos;
}

CEventFireSound::CEventFireSound(irr::ode::CSerializer *pData) {
  pData->resetBufferPos();
  irr::u16 iCode=pData->getU16();
  if (iCode==EVENT_FIRE_SND_ID) {
    m_iSound=pData->getU8();
    m_fVolume=pData->getF32();
    pData->getVector3df(m_vPos);
  }
}

const irr::c8 *CEventFireSound::toString() {
  sprintf(m_sString,"CEventFireSound");
  return m_sString;
}

irr::ode::CSerializer *CEventFireSound::serialize() {
  if (m_pSerializer==NULL) {
    m_pSerializer=new irr::ode::CSerializer();
    m_pSerializer->addU16(EVENT_FIRE_SND_ID);
    m_pSerializer->addU8(m_iSound);
    m_pSerializer->addF32(m_fVolume);
    m_pSerializer->addVector3df(m_vPos);
  }
  return m_pSerializer;
}

irr::ode::IIrrOdeEvent *CEventFireSound::clone() {
  return new CEventFireSound((enSound)m_iSound,m_fVolume,m_vPos);
}

CEventHeliState::CEventHeliState() {
  m_fSound=0.0f;
  m_fThrust = 0.0f;
  m_bAutoPilot = false;
}

CEventHeliState::CEventHeliState(irr::s32 iNodeId, irr::f32 fSound, bool bAutoPilot, irr::f32 fThrust) {
  m_iNodeId=iNodeId;
  m_fSound=fSound;
  m_bAutoPilot = bAutoPilot;
  m_fThrust = fThrust;
}

CEventHeliState::CEventHeliState(irr::ode::CSerializer *pData) {
  pData->resetBufferPos();
  irr::u16 iCode=pData->getU16();
  if (iCode==EVENT_HELI_STATE_ID) {
    m_iNodeId=pData->getS32();
    m_fSound=pData->getF32();
    m_bAutoPilot = pData->getU8()!=0;
    m_fThrust = pData->getF32();
  }
}

const irr::c8 *CEventHeliState::toString() {
  sprintf(m_sString,"CEventHeliState");
  return m_sString;
}

irr::ode::CSerializer *CEventHeliState::serialize() {
  if (m_pSerializer==NULL) {
    m_pSerializer=new irr::ode::CSerializer();
    m_pSerializer->addU16(EVENT_HELI_STATE_ID);
    m_pSerializer->addS32(m_iNodeId);
    m_pSerializer->addF32(m_fSound);
    m_pSerializer->addU8(m_bAutoPilot?1:0);
    m_pSerializer->addF32(m_fThrust);
  }
  return m_pSerializer;
}

irr::ode::IIrrOdeEvent *CEventHeliState::clone() {
  return new CEventHeliState(m_iNodeId,m_fSound,m_bAutoPilot,m_fThrust);
}
