  #include <CEventRudderPositions.h>

CEventRudderPositions::CEventRudderPositions() {
  m_pSerializer=NULL;
  m_fYaw=0.0f;
  m_fPitch=0.0f;
  m_fRoll=0.0f;
}

CEventRudderPositions::CEventRudderPositions(irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll) {
  m_pSerializer=NULL;
  m_fYaw=fYaw;
  m_fPitch=fPitch;
  m_fRoll=fRoll;
}

CEventRudderPositions::CEventRudderPositions(irr::ode::CSerializer *pData) {
  pData->resetBufferPos();
  irr::u16 iCode=pData->getU16();
  if (iCode==EVENT_RUDDER_ID) {
    m_fYaw=pData->getF32();
    m_fPitch=pData->getF32();
    m_fRoll=pData->getF32();
  }
}

irr::ode::CSerializer *CEventRudderPositions::serialize() {
  if (m_pSerializer==NULL) {
    m_pSerializer=new irr::ode::CSerializer();
    m_pSerializer->addU16(EVENT_RUDDER_ID);
    m_pSerializer->addF32(m_fYaw);
    m_pSerializer->addF32(m_fPitch);
    m_pSerializer->addF32(m_fRoll);
  }
  return m_pSerializer;
}

const irr::c8 *CEventRudderPositions::toString() {
  sprintf(m_sString,"CEventRudderPositions: yaw=%.2f, pitch=%.2f, roll=%.2f",m_fYaw,m_fPitch,m_fRoll);
  return m_sString;
}

irr::ode::IIrrOdeEvent *CEventRudderPositions::clone() {
  return new CEventRudderPositions(m_fYaw,m_fPitch,m_fRoll);
}
