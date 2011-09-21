#ifndef _I_IRR_ODE_EVENT_WRITER
  #define _I_IRR_ODE_EVENT_WRITER

  #include <CIrrOdeManager.h>

namespace irr {
  namespace ode {
    enum eEventWriterType {
      eIrrOdeEventWriterUnknown,    /**<! use this for all event writers that are not handles by the IrrOdeDevice */
      eIrrOdeEventWriterBody,
      eIrrOdeEventWriterJointHinge,
      eIrrOdeEventWriterJointHinge2
    };

    class IIrrOdeEvent;

    class IIrrOdeEventWriter {
      protected:
        CIrrOdeManager *m_pOdeEventManager;

      public:
        IIrrOdeEventWriter() {
          m_pOdeEventManager=CIrrOdeManager::getSharedInstance();
        }

        virtual ~IIrrOdeEventWriter() {
        }

        virtual IIrrOdeEvent *writeEvent()=0;

        void dataChanged() {
          m_pOdeEventManager->objectChanged(this);
        }

        virtual eEventWriterType getEventWriterType()=0;
    };

  }
}
#endif
