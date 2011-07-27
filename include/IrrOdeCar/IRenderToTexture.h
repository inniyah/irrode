#ifndef _I_RENDER_TO_TEXTURE
  #define _I_RENDER_TO_TEXTURE

  #include <irrlicht.h>

using namespace irr;

class IRenderToTexture {
  protected:
    IrrlichtDevice *m_pDevice;
    gui::IGUIEnvironment *m_pGuienv;
    video::IVideoDriver  *m_pDrv;
    scene::ISceneManager *m_pSmgr;
    core::stringc         m_sName;
    video::ITexture      *m_pTarget;

    u32 processTextureReplace(scene::ISceneNode *pNode) {
      u32 iRet=0;

      core::list<scene::ISceneNode *> lChildren=pNode->getChildren();
      core::list<scene::ISceneNode *>::Iterator it;

      for (u32 i=0; i<pNode->getMaterialCount(); i++) {
        for (u32 j=0; j<4; j++) {
          video::ITexture *p=pNode->getMaterial(i).getTexture(j);
          if (p!=NULL) {
            core::stringc s=pNode->getMaterial(i).getTexture(j)->getName();
            s.make_lower();
            if (s.find(m_sName.c_str())!=-1) {
              pNode->getMaterial(i).setTexture(j,m_pTarget);
              iRet++;
            }
          }
        }
      }

      for (it=lChildren.begin(); it!=lChildren.end(); it++) iRet+=processTextureReplace(*it);
      return iRet;
    }

    void startRttUpdate() { m_pDrv->setRenderTarget(m_pTarget,true,true,video::SColor(0xFF,0xFF,0xD0,0x80)); }
    void endRttUpdate  () { m_pDrv->setRenderTarget(video::ERT_FRAME_BUFFER); }

  public:
    IRenderToTexture(IrrlichtDevice *pDevice, const char *sName, core::dimension2du cSize) {
      m_pDevice=pDevice;

      m_pGuienv=m_pDevice->getGUIEnvironment();
      m_pDrv=m_pDevice->getVideoDriver();
      m_pSmgr=m_pDevice->getSceneManager();

      m_sName=sName;
      m_sName.make_lower();

      m_pTarget=m_pDrv->addRenderTargetTexture(cSize);
    }

    virtual ~IRenderToTexture() {
      m_pTarget->drop();
    }

    virtual void update(bool bPlane)=0;

    video::ITexture *getTexture() {
      return m_pTarget;
    }
};

#endif


