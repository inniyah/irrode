#ifndef _C_IRR_ODE_RECORDER
  #define _C_IRR_ODE_RECORDER

  #include <irrlicht.h>

  #include <event/IIrrOdeEventListener.h>
  #include <event/IIrrOdeEvent.h>

namespace irr { namespace ode {


class CIrrOdeRecorder : public IIrrOdeEventListener {
  protected:
    IrrlichtDevice *m_pDevice;
    bool m_bRecording;
    irr::core::list<IIrrOdeEvent *> m_lEvents;
    stringc m_sAppName;

  public:
    CIrrOdeRecorder(IrrlichtDevice *pDevice);
    ~CIrrOdeRecorder();

    void install(const c8 *sAppName);
    void destall();

    void startRecording();
    void stopRecording();

    void saveRecording(const c8 *sFileName);

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);
};

} } //namespaces

#endif
