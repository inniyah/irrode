  #include <irrlicht.h>
  #include <CRearView.h>
  #include <CGUINeedleIndicator.h>

CRearView::CRearView(irr::IrrlichtDevice *pDevice, const char *sName, irr::scene::ICameraSceneNode *pCam, irr::scene::ISceneNode *pNode) : IRenderToTexture(pDevice,sName,irr::core::dimension2d<irr::u32>(256,256)) {
  m_pCam=pCam;
  m_pDevice=pDevice;
  irr::u32 iReplace=processTextureReplace(pNode);
  printf("**** CRearView: replaced %i texture.\n",iReplace);
  m_bActive = false;
}

CRearView::~CRearView() {
}

void CRearView::update() {
  if (!m_bActive) return;

  startRttUpdate();
  m_pSmgr->setActiveCamera(m_pCam);
  m_pSmgr->drawAll();
  endRttUpdate();
}

void CRearView::setCameraParameters(const irr::core::vector3df cPos, const irr::core::vector3df cTgt, const irr::core::vector3df cUp) {
  if (!m_bActive) return;

  m_pCam->setPosition(cPos);
  m_pCam->setTarget(cTgt);
  m_pCam->setUpVector(cUp);
}
