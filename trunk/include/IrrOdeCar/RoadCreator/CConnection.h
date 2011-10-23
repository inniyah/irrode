#ifndef _C_CONNECTION
  #define _C_CONNECTION

  #include <irrlicht.h>
  #include <INotification.h>
  #include <IRoadPart.h>

class CSegment;
class CTextureParameters;

using namespace irr;

/**
 * @class CConnection
 * @author Christian Keimel / bulletbyte.de
 * This class represents the connections between two segments. The
 * shape of the connection is calculated by Bezier's algorithm
 */
class CConnection : public INotification, public IRoadPart {
  public:
    /**
     * Type of Bezier algorithm
     */
    enum eConnectionType {
      eBezier1,
      eBezier2,
      eBezier3
    };

  protected:
    CSegment *m_pSegment1,  /**<! the first connected segment */
             *m_pSegment2;  /**<! the second connected segment */

    core::stringc m_sSegment1,  /**<! name of the first connected segment, mainly for saving */
                  m_sSegment2;  /**<! name of the second connected segment, mainly for saving */

    eConnectionType m_eType;  /**<! type of the connection */

    u32 m_iSegment1Border,  /**<! number of the used border of the first segment (1..4) */
        m_iSegment2Border,  /**<! number of the used border of the second segment (1..4) */
        m_iSteps;           /**<! number of steps for the calculation */
    s32 m_iSelectedPoint;   /**<! number of the selected point (for editing) */
    f32 m_fTexX,            /**<! texture calculation buffer */
        m_fOffset,          /**<! offset (i.e. height) of the connection */
        m_fRoadWidth,       /**<! highest road width of this connectior (for texture calculations) */
        m_fHpOff[4],        /**<! offset of the helppoints */
        m_fWallHeight;      /**<! height of the surrounding walls */

    bool m_bFlipConnection, /**<! flip one of the connections. Needed from time to time to connect correctly */
         m_bFlipVertices,   /**<! flip the vertices. Sometimes it happens that we see the backface */
         m_bSelected,       /**<! is this connection selected (for editing)? */
         m_bWalls[2];       /**<! add wall to side? */

    CTextureParameters *m_pTexParams[6];  /**<! the texture parameters for the four available sides */

    core::vector3df m_vHelpPoints[4], /**<! the four help points for Bezier3, Bezier2 uses Nr. 0 and 2, Bezier1 uses none */
                    m_vDraw[8];
    core::aabbox3df m_cBox;

    IrrlichtDevice *m_pDevice;
    video::IVideoDriver *m_pDrv;    /**<! the Irrlicht video driver */
    io::IFileSystem *m_pFs;

    scene::IMeshBuffer *m_pMeshBuffer[8]; /**<! the mesh buffers of the connection */

    /**
     * Init the Bezier helppoints
     * @see CConnection::setSegment1
     * @see CConnection::setSegment2
     * @see CConnection::setSegment1Border
     * @see CConnection::setSegment2Border
     * @see CConnection::setType
     */
    void calculateHelpPoints();

    /**
     * Recalculates the meshbuffer after some attribute was changed
     * @see CConnection::update
     */
    virtual void recalcMeshBuffer();

    /**
     * Get the next point using the Bezier1 algorithm
     * @param p array with the two points for the algorithm
     * @param fStep step number (0..1)
     * @return the next point
     * @see recalcMeshBuffer
     * @see render
     */
    core::vector3df getBezier1(core::vector3df p[], f32 fStep);

    /**
     * Get the next point using the Bezier2 algorithm
     * @param p array with the three points (start, help, end) for the algorithm
     * @param fStep step number (0..1)
     * @return the next point
     * @see recalcMeshBuffer
     * @see render
     */
    core::vector3df getBezier2(core::vector3df p[], f32 fStep);

    /**
     * Get the next point using the Bezier3 algorithm
     * @param p array with the four points (start, help1, help2, end) for the algorithm
     * @param fStep step number (0..1)
     * @return the next point
     * @see recalcMeshBuffer
     * @see render
     */
    core::vector3df getBezier3(core::vector3df p[], f32 fStep);

    /**
     * This method creates vertices out of the four vectors in the first argument
     * and adds those to the array and their indices to the index array
     * @param v the input vectors
     * @param aVerts the output array. The input vectors will be added to this array
     * @param aIdx the index output. The indices of the vertex output array are stored here
     * @param b boolean for multi-colored appearance in the editor
     * @param iIdx index of the side that the vectors are for (road, bottom, left, right)
     * @see CConnection::recalcMeshBuffer
     */
    void addToBuffers(core::vector3df v[], core::array<video::S3DVertex> &aVerts, core::array<u16> &aIdx, bool b, u32 iIdx);

    void addToBuffersWall(core::vector3df v[], core::array<video::S3DVertex> &aVerts, core::array<u16> &aIdx, bool b, bool bRotate, bool bBasement, CTextureParameters *pTex);

    /**
     * Using this method you can add a single vertex to a temporarily used array of vertices. This array
     * is searched to see if the position of any vertex inside mathes the new vertex, and if this is true
     * the normals of those vertices are interpolated
     * @param vtx the vertex to add
     * @param aTmp the array of vertices
     * @return the index of the newly added vertex
     * @see CConnection::addToBuffers
     */
    u16 addToTempVertexBuffer(video::S3DVertex vtx, core::array<video::S3DVertex> &aTmp);

  public:
    /**
     * Constructor
     * @param pDrv used Irrlicht videodriver
     * @param pInitTexture initial texture paramters
     */
    CConnection(IrrlichtDevice *pDevice, CTextureParameters *pInitTexture);

    /**
     * The destructor
     */
    virtual ~CConnection();

    void setSegment1(CSegment *p);  /**<! set the first segment for this connection */
    void setSegment2(CSegment *p);  /**<! set the second segment for this connection */

    void setSegment1NoInit(CSegment *p);  /**<! set the first segment for this connection without initialization*/
    void setSegment2NoInit(CSegment *p);  /**<! set the second segment for this connection without initialization*/

    void setSegment1Border(u32 i);  /**<! set the index of the border used from the first segment (0..3) */
    void setSegment2Border(u32 i);  /**<! set the index of the border used from the second segment (0..3) */

    void setType(eConnectionType eType);  /**<! set the type of connection (Bezier1, Bezier2, Bezier3 */

    void setNumberOfSteps(u32 i); /**< Set the number of steps used for this connection */

    /**
     * Modify a helppoint
     * @param i index of the helppoint (0..3)
     * @param v the new vector
     */
    void setHelpPoint(u32 i, core::vector3df v);

    void setFlipConnection(bool b); /**<! set the "flip connection" flag */
    void setFlipVertices  (bool b); /**<! set the "flip vertices" flag */
    void setSelected      (bool b); /**<! set the "selected" flag (for editing) */

    CSegment *getSegment1();  /**<! query the first segment */
    CSegment *getSegment2();  /**<! query the second segment */

    u32 getSegment1Border();    /**<! query the index of the used border from the first segment */
    u32 getSegment2Border();    /**<! query the index of the used border from the second segment */

    eConnectionType getConnectionType();  /**<! get the connection type */

    u32 getNumberOfSteps(); /**<! get the number of steps */

    /**
     * Get a helppoint
     * @param i the index of the helppoint (0..3)
     * @return the helppoint
     */
    core::vector3df getHelpPoint(u32 i);

    bool getFlipConnection();   /**<! query the "flip connection" flag */
    bool getFlipVertices();     /**<! query the "flip vertices" flag */

    void setOffset(f32 f);    /**< set the offset (i.e. base height) of the connection */
    f32 getOffset();          /**< get the offset (i.e. base height) of the connection */

    virtual void render();    /**< render the connection (for the editor) */

    void setSelectedPoint(s32 iPoint);  /**< select one of the helppoints */

    virtual void save(io::IAttributes *out);    /**< save this connection */
    virtual void load(io::IAttributes *in );    /**< load this connection */

    const core::stringc &getSegment1Name(); /**<! get the name of the first segment */
    const core::stringc &getSegment2Name(); /**<! get the name of the second segment */

    /**
     * Callback method that is called when an attribute from one of the connected
     * segments has changed
     * @param p the segment that called the method
     * @see CConnection::setSegment1NoInit
     * @see CConnection::setSegment2NoInit
     * @see CSegment::attributeChanged
     */
    virtual void attributesChanged(CSegment *p);

    /**
     * Callback method that is called when one of the connected
     * segments is deleted
     * @param p the segment that will be deleted
     * @see CSegment::~CSegment()
     * @see CConnection::setSegment1Border
     * @see CConnection::setSegment2Border
     * @see CConnection::setNumberOfSteps
     * @see CConnection::setOffset
     * @see CConnection::setHelpPoint
     * @see CConnection::setFlipConnection
     * @see CConnection::setFlipVertices
     * @see CConnection::attributesChanged
     * @see CConnection::objectDeleted
     * @see CConnectionParameterWindow::textureModified
     * @see CRunner::loadRoad
     */
    virtual void objectDeleted(CSegment *p);

    /**
     * This method is called when something changed. It triggers a recalculation of the meshbuffers
     * @see CConnection::recalcMeshBuffer
     */
    void update();

    /**
     * Get one of the texture parameters structures
     * @param i the index of the structure (0..3)
     * @return the structur
     */
    virtual CTextureParameters *getTextureParameters(u32 i);

    virtual u32 getTextureCount() { return 4; }

    /**
     * Get one of the meshbuffers
     * @param i the index of the meshbuffer (0..3)
     * @return the meshbuffer
     */
    virtual scene::IMeshBuffer *getMeshBuffer(u32 i);

    void setHpOffset(u32 iHp, f32 fOff) {
      if (iHp<4) {
        m_fHpOff[iHp]=fOff;
        calculateHelpPoints();
        update();
      }
    }

    f32 getHpOffset(u32 iHp) { return iHp<4?m_fHpOff[iHp]:0.0f; }

    void setWallFlag(u32 iIdx, bool b) { if (iIdx<2) { m_bWalls[iIdx]=b; update(); } }
    void setWallHeight(f32 f) { m_fWallHeight=f; update(); }

    bool getWallFlag(u32 idx) { return idx<2?m_bWalls[idx]:false; }
    f32 getWallHeight() { return m_fWallHeight; }

    virtual s32 getNumberOfMeshBuffers() {
      return 8;
    }
};

#endif
