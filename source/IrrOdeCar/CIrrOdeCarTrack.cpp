  #include <CIrrOdeCarTrack.h>
  #include <CEventVehicleState.h>

CIrrOdeCarTrack::CIrrOdeCarTrack(irr::ode::CIrrOdeBody *pBody) {
  m_iBodyId = pBody->getID();
  m_iNextCp = -1;
  m_bPlane = strstr(pBody->getName(), "plane");
  m_iCurStep = 0;
  m_iLastLapStep = 0;
  printf("%i: %s\n",m_iBodyId, m_bPlane?"Plane":"Car");

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
}

CIrrOdeCarTrack::~CIrrOdeCarTrack() {
}

bool CIrrOdeCarTrack::onEvent(ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    m_iCurStep++;
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventTrigger) {
    irr::ode::CIrrOdeEventTrigger *pTrig=(irr::ode::CIrrOdeEventTrigger *)pEvent;

    if ((pTrig->getTriggerId() < 1000 && m_bPlane) || (pTrig->getTriggerId() >= 1000 && !m_bPlane)) {
      return false;
    }

    if (pTrig->getBodyId()==m_iBodyId) {
      if (pTrig->getTriggerId()>=100) {
        s32 iId = pTrig->getTriggerId() % 100;
        if (iId==0) {
          if (m_iNextCp==-1) {
            CEventLapTime *p=new CEventLapTime(0,m_iBodyId,0);
            irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
            printf("\t\tStart of Lap: %.2f\n",(((float)m_iCurStep)-((float)m_iLastLapStep))*0.016f);
          }
          else
            if (m_iNextCp==0) {
              CEventLapTime *p=new CEventLapTime((((float)m_iCurStep)-((float)m_iLastLapStep))*0.016f,m_iBodyId,0);
              irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
              //m_pCockpit->setLastLapTime((((float)m_iCurStep)-((float)m_iLastLapStep))*0.016f);
              //m_pCockpit->setSplitTime(0.0f);
            }
          m_iNextCp=1;
          m_iLastLapStep=m_iCurStep;
        }
        else
          if (iId==50) {
            if (m_iNextCp!=0) {
              CEventLapTime *p=new CEventLapTime((((float)m_iCurStep)-((float)m_iLastLapStep))*0.016f,m_iBodyId,1);
              irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
              //m_pCockpit->setSplitTime((((float)m_iCurStep)-((float)m_iLastLapStep))*0.016f);
            }
            m_iNextCp=0;
          }
          else
            if (iId==99) {
              CEventLapTime *p=new CEventLapTime(0,m_iBodyId,3);
              irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
              //m_pCockpit->cancelLap();
              m_iNextCp=-1;
            }
            else {
              if (m_iNextCp==iId) {
                CEventLapTime *p=new CEventLapTime((((float)m_iCurStep)-((float)m_iLastLapStep))*0.016f,m_iBodyId,2);
                irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
                //m_pCockpit->setSplitTime((((float)m_iCurStep)-((float)m_iLastLapStep))*0.016f);
                m_iNextCp++;
              }
            }
      }
    }
  }

  return true;
}

bool CIrrOdeCarTrack::handlesEvent(ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventTrigger || pEvent->getType()==irr::ode::eIrrOdeEventStep;
}
