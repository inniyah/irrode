#ifndef _I_IRR_ODE_EVENT_WRITER
  #define _I_IRR_ODE_EVENT_WRITER

  #include <CIrrOdeManager.h>

namespace irr {
  namespace ode {
    enum eEventWriterType {
      eIrrOdeEventWriterUnknown,    /**<! use this for all event writers that are not handles by the IrrOdeDevice */
      eIrrOdeEventWriterBody,
      eIrrOdeEventWriterJointHinge,
      eIrrOdeEventWriterJointHinge2,
      eIrrOdeEventWriterJointSlider
    };

    class IIrrOdeEvent;

    /**
     * @class IIrrOdeEventWriter
     * This is an interface that should be implemented for objects that write custom events.
     * Whenever the state has changed the "dataChanged" method must be called, and IrrOde
     * will query all changed objects for their update events after the next step has been done.
     * @author Christian Keimel / bulletbyte.de
     */
    class IIrrOdeEventWriter {
      protected:
        CIrrOdeManager *m_pOdeEventManager;

      public:
        IIrrOdeEventWriter() {
          m_pOdeEventManager=CIrrOdeManager::getSharedInstance();
        }

        virtual ~IIrrOdeEventWriter() {
        }

        void dataChanged() {
          m_pOdeEventManager->objectChanged(this);
        }

        /**
         * Implement this method to write the event you want to send
         * @return the state update event
         */
        virtual ode::IIrrOdeEvent *writeEvent()=0;

        /**
         * Implement this method and return "eIrrOdeEventWriterUnknown" to make sure
         * the "writeEvent" method will be called. The IrrOde internal event writers
         * are directly handled by the IrrOde device, and the writeEvent method
         * is not called.
         */
        virtual ode::eEventWriterType getEventWriterType()=0;
    };

  }
}
#endif
