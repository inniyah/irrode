  #include <CEventVehicleState.h>

CEventRudderPositions::CEventRudderPositions() {
  m_pSerializer=NULL;
  m_iNodeId=0;
  m_iYaw=0;
  m_iPitch=0;
  m_iRoll=0;
}

CEventRudderPositions::CEventRudderPositions(irr::s32 iId, irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll, bool bThreeWheeler) {
  m_iNodeId=iId;
  m_pSerializer=NULL;
  m_iYaw=(irr::u8)(fYaw*100.0f);
  m_iPitch=(irr::u8)(fPitch*100.0f);
  m_iRoll=(irr::u8)(fRoll*100.0f);
  m_bThreeWheeler=bThreeWheeler;
}

CEventRudderPositions::CEventRudderPositions(irr::ode::CSerializer *pData) {
  pData->resetBufferPos();
  irr::u16 iCode=pData->getU16();
  if (iCode==EVENT_RUDDER_ID) {
    m_iNodeId=pData->getS32();
    m_iYaw=pData->getS8();
    m_iPitch=pData->getS8();
    m_iRoll=pData->getS8();
    m_bThreeWheeler=pData->getU8();
  }
}

irr::ode::CSerializer *CEventRudderPositions::serialize() {
  if (m_pSerializer==NULL) {
    m_pSerializer=new irr::ode::CSerializer();
    m_pSerializer->addU16(EVENT_RUDDER_ID);
    m_pSerializer->addS32(m_iNodeId);
    m_pSerializer->addS8(m_iYaw);
    m_pSerializer->addS8(m_iPitch);
    m_pSerializer->addS8(m_iRoll);
    m_pSerializer->addU8(m_bThreeWheeler?1:0);
  }
  return m_pSerializer;
}

const irr::c8 *CEventRudderPositions::toString() {
  sprintf(m_sString,"CEventRudderPositions (%i): yaw=%i, pitch=%i, roll=%i",m_iNodeId,m_iYaw,m_iPitch,m_iRoll);
  return m_sString;
}

irr::ode::IIrrOdeEvent *CEventRudderPositions::clone() {
  return new CEventRudderPositions(m_iNodeId,((irr::f32)m_iYaw)/100.0f,((irr::f32)m_iPitch)/100.0f,((irr::f32)m_iRoll)/100.0f,m_bThreeWheeler);
}

CEventTankState::CEventTankState() {
  m_iNodeId=0;
  for (irr::u32 i=0; i<4; i++) m_aAngles[i]=0;
  m_fCannonAngle=0.0f;
}

CEventTankState::CEventTankState(irr::s32 iId, const irr::s8 *aAngles, irr::f32 fCannonAngle, irr::f32 fTurretAngle) {
  m_iNodeId=iId;
  for (irr::u32 i=0; i<4; i++) m_aAngles[i]=aAngles[i];
  m_fCannonAngle=fCannonAngle;
  m_fTurretAngle=fTurretAngle;
}

CEventTankState::CEventTankState(irr::ode::CSerializer *pData) {
  pData->resetBufferPos();
  irr::u16 iCode=pData->getU16();
  if (iCode==EVENT_TANK_STATE_ID) {
    m_iNodeId=pData->getS32();
    for (irr::u32 i=0; i<4; i++) m_aAngles[i]=pData->getS8();
  }
  m_fCannonAngle=pData->getF32();
  m_fTurretAngle=pData->getF32();
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
  }
  return m_pSerializer;
}

const irr::c8 *CEventTankState::toString() {
  sprintf(m_sString,"CEventTankState: %i, %i, %i, %i, %i, %.2f, %.2f",m_iNodeId,m_aAngles[0],m_aAngles[1],m_aAngles[2],m_aAngles[3],m_fCannonAngle,m_fTurretAngle);
  return m_sString;
}

irr::ode::IIrrOdeEvent *CEventTankState::clone() {
  return new CEventTankState(m_iNodeId,m_aAngles,m_fCannonAngle,m_fTurretAngle);
}
