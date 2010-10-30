#ifndef _C_IRR_ODE_REPLAY_LOADER
  #define _C_IRR_ODE_REPLAY_LOADER

  #include <event/IIrrOdeEvent.h>
  #include <CSerializer.h>

class CIrrOdeReplayLoader {
  protected:
    irr::core::list<irr::ode::IIrrOdeEvent *> m_lEvents;
    irr::IrrlichtDevice *m_pDevice;
    irr::scene::ISceneManager *m_pSmgr;
    irr::core::stringc m_sAppName,
                       m_sSceneFile;
    bool m_bValidReplay;
    irr::ode::CSerializer m_cSerializer;					  /**< the serializer for the messages */

    void deleteAllMessages();

  public:
    CIrrOdeReplayLoader(irr::IrrlichtDevice *pDevice, const char *sReplay);
    ~CIrrOdeReplayLoader();
};

#endif
