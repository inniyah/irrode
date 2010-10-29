  #include <observer/CIrrOdeRecorder.h>
  #include <observer/CIrrOdeWorldObserver.h>
  #include <event/CIrrOdeEventIdentify.h>

namespace irr {
namespace ode {

CIrrOdeRecorder::CIrrOdeRecorder(IrrlichtDevice *pDevice) {
  m_bRecording=false;
  m_pDevice=pDevice;
  m_sAppName="";
}

CIrrOdeRecorder::~CIrrOdeRecorder() {
}

void CIrrOdeRecorder::install(const c8 *sAppName) {
  m_sAppName=sAppName;
  CIrrOdeWorldObserver::getSharedInstance()->addEventListener(this);
}

void CIrrOdeRecorder::destall() {
  CIrrOdeWorldObserver::getSharedInstance()->removeEventListener(this);
}

void CIrrOdeRecorder::startRecording() {
  CIrrOdeEventIdentify *p=new CIrrOdeEventIdentify(m_sAppName.c_str());
  m_lEvents.push_back(p);
  m_bRecording=true;
}

void CIrrOdeRecorder::stopRecording() {
  m_bRecording=false;
}

void CIrrOdeRecorder::saveRecording(const c8 *sFileName) {
}

bool CIrrOdeRecorder::onEvent(IIrrOdeEvent *pEvent) {
  if (!m_bRecording) return false;
  m_lEvents.push_back(pEvent->clone());
  return true;
}

bool CIrrOdeRecorder::handlesEvent(IIrrOdeEvent *pEvent) {
  return true;
}

}
}
