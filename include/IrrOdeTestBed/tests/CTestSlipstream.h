#ifndef _C_TEST_SLIPSTREAM
  #define _C_TEST_SLIPSTREAM

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

using namespace irr;

class CSlipStream {
  protected:
    ode::CIrrOdeBody *m_pSource;
    s32 m_iTtl,m_iInitTtl;
    f32 m_fPwr,m_fSize,m_fGrow,m_fInitPwr;
    core::vector3df m_vDir,m_vPos;
    core::list<ode::CIrrOdeBody *> m_lBodies;

  public:
    CSlipStream(ode::CIrrOdeBody *pBody, s32 iTtl, f32 fPwr, f32 fGrow, core::list<ode::CIrrOdeBody *> lBodies) {
      m_pSource=pBody;
      m_iInitTtl=iTtl;
      m_fInitPwr=fPwr;
      m_fGrow=fGrow;

      init();

      m_iTtl=0;

      core::list<ode::CIrrOdeBody *>::Iterator it;
      for (it=lBodies.begin(); it!=lBodies.end(); it++)
        if ((*it)!=pBody) m_lBodies.push_back(*it);
    }

    void init() {
      m_iTtl=m_iInitTtl;
      m_fPwr=m_pSource->getLinearVelocity().getLength()/5.0f*m_fInitPwr;
      m_fSize=0.0f;

      m_vDir=m_pSource->getLinearVelocity();
      m_vDir.normalize();
      m_vPos=m_pSource->getPosition();
    }

    void step() {
      if (m_iTtl<=0) return;

      core::list<ode::CIrrOdeBody *>::Iterator it;
      for (it=m_lBodies.begin(); it!=m_lBodies.end(); it++) {
        ode::CIrrOdeBody *p=(ode::CIrrOdeBody *)(*it);
        core::vector3df pos=p->getPosition();
        if ((pos-m_vPos).getLength()<m_fSize) {
          p->addForce(m_fPwr*m_vDir);
        }
      }

      m_fPwr-=m_fInitPwr/m_iTtl;
      m_fSize+=m_fGrow;
      m_iTtl--;
    }
};


/**
 * @class CTestSlipstream
 * Test for slipstream functionality
 */
class CTestSlipstream : public IState, public ode::IIrrOdeEventListener {
  protected:
    ode::CIrrOdeBody *m_pMarble;
    core::list<ode::CIrrOdeBody *> m_lBodies;

    CSlipStream *m_aSlipStream[30];
    s32 m_iCurrent;

  public:
    CTestSlipstream(IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestSlipstream() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual s32 update();

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);
};

#endif


