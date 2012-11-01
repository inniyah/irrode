#ifndef _C_IRR_ODE_FAKE_THREAD_H
  #define _C_IRR_ODE_FAKE_THREAD_H

  #include <thread/IThread.h>

namespace irr {
  namespace ode {
    class IIrrOdeEventQueue;

    /**
     * @class CFakeThread
     * This is a fake thread that allows using the same interfaces for both
     * multi- and single-threaded environments
     * @author Christian Keimel / bulletbyte.de
     */
    class CFakeThread : public IThread {
      protected:
        irr::ode::IIrrOdeEventQueue *m_pInputQueue,
                                    *m_pOutputQueue;

      public:
        CFakeThread();

        virtual ~CFakeThread();

        virtual irr::ode::IIrrOdeEventQueue *getInputQueue();
        virtual irr::ode::IIrrOdeEventQueue *getOutputQueue();
    };
  }
}

#endif
