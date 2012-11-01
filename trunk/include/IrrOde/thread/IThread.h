#ifndef _I_IRR_ODE_THREAD_H
  #define _I_IRR_ODE_THREAD_H

  //#include <irrlicht.h>
  #include <event/IIrrOdeEventListener.h>
  #include <event/IIrrOdeEventQueue.h>

namespace irr {
  namespace ode {

    class IThread : public irr::ode::IIrrOdeEventListener {
      public:
        virtual ~IThread() { }

        /**
         * Get the thread's input queue. All events posted to this
         * queue get processed within the thread
         * @return the thread's input queue
         */
        virtual irr::ode::IIrrOdeEventQueue *getInputQueue()=0;

        /**
         * Get the thread's output queue. If another thread is interested
         * in the event this thread creates it must register itself to this
         * output queue, the events will be posted to the other threads
         * which write them to their input queue
         * @return the thread's output queue
         */
        virtual irr::ode::IIrrOdeEventQueue *getOutputQueue()=0;

        /**
         * This is the event handler that is called by other thread's output
         * queues.
         * @param pEvent the posted event
         * @return true as the thread processes all events
         */
        virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent) {
          irr::ode::IIrrOdeEvent *p = pEvent->clone();
          if (p != NULL) getInputQueue()->postEvent(p);
          return true;
        }

        /**
         * The thread handles all events that get posted
         * @return true
         */
        virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
          return true;
        }
    };
  }
}

#endif
