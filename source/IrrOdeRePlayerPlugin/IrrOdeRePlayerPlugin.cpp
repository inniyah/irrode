  #include <IrrOdeRePLayerPlugin.h>

  #include <CAdvancedParticleSystemNode.h>

static CAdvancedParticleSystemNodeFactory *g_pFactory=NULL;

int DLL_EXPORT install(irr::IrrlichtDevice *pDevice) {
  printf("Plugin installed -- pDevice=%i\n",(int)pDevice);
  g_pFactory=new CAdvancedParticleSystemNodeFactory(pDevice->getSceneManager());
  pDevice->getSceneManager()->registerSceneNodeFactory(g_pFactory);
  return 0;
}

int DLL_EXPORT destall(irr::IrrlichtDevice *pDevice) {
  printf("Plugin destalled\n");
  return 0;
}
