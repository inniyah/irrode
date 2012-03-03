  #include <irrlicht.h>
  #include <CRearView.h>
  #include <CGUINeedleIndicator.h>

CRearView::CRearView(irr::IrrlichtDevice *pDevice, const char *sName, irr::scene::ICameraSceneNode *pCam) : IRenderToTexture(pDevice,sName,irr::core::dimension2d<irr::u32>(256,256)) {
  m_pCam=pCam;
  m_pDevice=pDevice;
  irr::u32 iReplace=processTextureReplace(m_pSmgr->getRootSceneNode());
  printf("**** CRearView: replaced %i texture.\n",iReplace);
}

CRearView::~CRearView() {
}

void CRearView::update(bool b) {
  startRttUpdate();
  irr::scene::ICameraSceneNode *pActCam=m_pSmgr->getActiveCamera();
  m_pSmgr->setActiveCamera(m_pCam);
  m_pSmgr->drawAll();
  endRttUpdate();
  m_pSmgr->setActiveCamera(pActCam);
}

void CRearView::setCameraParameters(const irr::core::vector3df cPos, const irr::core::vector3df cTgt, const irr::core::vector3df cUp) {
  m_pCam->setPosition(cPos);
  m_pCam->setTarget(cTgt);
  m_pCam->setUpVector(cUp);
}
