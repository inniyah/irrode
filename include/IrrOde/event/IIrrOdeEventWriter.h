#ifndef _I_IRR_ODE_EVENT_WRITER
  #define _I_IRR_ODE_EVENT_WRITER

  #include <CIrrOdeManager.h>

namespace irr {
  namespace ode {
    enum eEventWriterType {
      eIrrOdeEventWriterUnknown,    /**<! use this for all event writers that are not handles by the IrrOdeDevice */
      eIrrOdeEventWriterBody,
      eIrrOdeEventWriterJointHinge
    };

    class IIrrOdeEvent;

    class IIrrOdeEventWriter {
      protected:
        CIrrOdeManager *m_pOdeEventManager;

      public:
        IIrrOdeEventWriter();

        virtual ~IIrrOdeEventWriter();

        virtual IIrrOdeEvent *writeEvent()=0;

        void dataChanged();

        virtual eEventWriterType getEventWriterType()=0;
    };

  }
}
#endif
