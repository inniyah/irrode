  #include <irrlicht.h>
  #include <thread/CFakeThread.h>
  #include <event/CIrrOdeEventQueue.h>

namespace irr {
  namespace ode {

    CFakeThread::CFakeThread() {
      m_pInputQueue  = new CIrrOdeEventQueue();
      m_pOutputQueue = new CIrrOdeEventQueue();
    }

    CFakeThread::~CFakeThread() {
      delete m_pInputQueue;
      delete m_pOutputQueue;
    }

    irr::ode::IIrrOdeEventQueue *CFakeThread::getInputQueue() {
      return m_pInputQueue;
    }

    irr::ode::IIrrOdeEventQueue *CFakeThread::getOutputQueue() {
      return m_pOutputQueue;
    }

  } //namespace ode
}   //namespace irr
