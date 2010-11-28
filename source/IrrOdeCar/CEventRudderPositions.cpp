  #include <CEventRudderPositions.h>

CEventRudderPositions::CEventRudderPositions() {
  m_pSerializer=NULL;
  m_iNodeId=0;
  m_iYaw=0;
  m_iPitch=0;
  m_iRoll=0;
}

CEventRudderPositions::CEventRudderPositions(irr::s32 iId, irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll) {
  m_iNodeId=iId;
  m_pSerializer=NULL;
  m_iYaw=(irr::u8)(fYaw*100.0f);
  m_iPitch=(irr::u8)(fPitch*100.0f);
  m_iRoll=(irr::u8)(fRoll*100.0f);
}

CEventRudderPositions::CEventRudderPositions(irr::ode::CSerializer *pData) {
  pData->resetBufferPos();
  irr::u16 iCode=pData->getU16();
  if (iCode==EVENT_RUDDER_ID) {
    m_iNodeId=pData->getS32();
    m_iYaw=pData->getS8();
    m_iPitch=pData->getS8();
    m_iRoll=pData->getS8();
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
  }
  return m_pSerializer;
}

const irr::c8 *CEventRudderPositions::toString() {
  sprintf(m_sString,"CEventRudderPositions (%i): yaw=%i, pitch=%i, roll=%i",m_iNodeId,m_iYaw,m_iPitch,m_iRoll);
  return m_sString;
}

irr::ode::IIrrOdeEvent *CEventRudderPositions::clone() {
  return new CEventRudderPositions(m_iNodeId,((irr::f32)m_iYaw)/100.0f,((irr::f32)m_iPitch)/100.0f,((irr::f32)m_iRoll)/100.0f);
}
