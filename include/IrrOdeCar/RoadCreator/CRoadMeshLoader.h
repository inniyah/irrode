#ifndef _C_ROAD_MESH_LOADER
  #define _C_ROAD_MESH_LOADER

  #include <irrlicht.h>

class CRoadMeshLoader : public irr::scene::IMeshLoader {
  protected:
    irr::IrrlichtDevice *m_pDevice;

  public:
    CRoadMeshLoader(irr::IrrlichtDevice *pDevice) { m_pDevice=pDevice; }

    virtual irr::scene::IAnimatedMesh *createMesh(irr::io::IReadFile *file);
    #ifdef _IRREDIT_PLUGIN
      virtual bool isALoadableFileExtension (const irr::c8 *filename) const;
    #else
      virtual bool isALoadableFileExtension (const irr::io::path &filename) const;
    #endif
    virtual ~CRoadMeshLoader() { }
};

#endif
