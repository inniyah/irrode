#ifndef _C_ROAD_LOADER
  #define _C_ROAD_LOADER

  #include <irrlicht.h>
  #include <CSurface.h>
  #include <CTextureParameters.h>

class CSegment;
class CSurface;
class CConnection;

class CRoadLoader {
  protected:
    irr::IrrlichtDevice *m_pDevice;

    irr::core::list<CSegment *> m_lSegments;         /**<! list of road segments */
    irr::core::list<CConnection *> m_lConnections;   /**<! list of connections */

    CSurface *m_pSurface;

    irr::core::vector3df m_vOfffset;     /**<! the Offset of the node (only relevant when shrinking the node) */
    bool m_bShrinkNode;

    CTextureParameters m_aSurfaceDefaults[_SURFACE_NUMBER_OF_BUFFERS];

    irr::core::stringc m_sCurrentRoad;               /**<! the currently opened road */

    void addBufferToArray(irr::scene::IMeshBuffer *p, irr::core::array<irr::scene::IMeshBuffer *> &aBuffers);

    bool sameMaterial(const irr::video::SMaterial &m1, const irr::video::SMaterial &m2);

  public:
    CRoadLoader(irr::IrrlichtDevice *pDevice);

    ~CRoadLoader() {
      if (m_pSurface!=NULL) delete m_pSurface;
    }

    void setSurfaceDefaultParameters(CTextureParameters *p) {
      if (p==NULL) return;
      for (irr::u32 i=0; i<_SURFACE_NUMBER_OF_BUFFERS; i++) p[i].copyTo(&m_aSurfaceDefaults[i]);
    }

    bool loadRoad(const irr::core::stringc sName);

    /**
     * This method saves a road to a file
     * @param sName the file to save the road to
     * @see CSegment::save
     * @see CConnection::save
     * @see CTextureParameters::save
     */
    void saveRoad();

    /**
     * Remove a road from memory
     */
    void destroyRoad();

    irr::core::list<CSegment    *> &getSegments   ();
    irr::core::list<CConnection *> &getConnections();

    const irr::core::stringc getCurrentRoadName();
    void setCurrentRoadName(const irr::core::stringc s);

    irr::scene::IAnimatedMesh *createMesh();

    bool deleteConnection(CConnection *pToDelete);

    bool deleteSegment(CSegment *pToDelete);

    CSurface *getSurface() {
      if (m_pSurface==NULL) m_pSurface=new CSurface(m_pDevice,m_aSurfaceDefaults);
      return m_pSurface;
    }

    void shrinkNode(bool b) { m_bShrinkNode=b; }
    bool didShrinkNode() { return m_bShrinkNode; }
    const irr::core::vector3df &getOffset() { return m_vOfffset; }
};

#endif
