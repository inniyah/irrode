  #include <CRoadMeshLoader.h>
  #include <CRoadLoader.h>

irr::scene::IAnimatedMesh *CRoadMeshLoader::createMesh(irr::io::IReadFile *file) {
  irr::core::stringc sName=file->getFileName();

  CRoadLoader *pLoader=new CRoadLoader(m_pDevice);
  pLoader->loadRoad(sName);

  return pLoader->createMesh();
}

#ifdef _IRREDIT_PLUGIN
  bool CRoadMeshLoader::isALoadableFileExtension (const irr::c8 *filename) const {
    const c8 *s=strrchr(filename,'.');
    return s!=NULL && strcmp(s,".road")==0;
  }
#else
  bool CRoadMeshLoader::isALoadableFileExtension (const irr::io::path &filename) const {
    return irr::core::hasFileExtension(filename,"Road","road");
  }
#endif
