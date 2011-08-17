#ifndef _C_SEGMENT
  #define _C_SEGMENT

  #include <irrlicht.h>
  #include <IRoadPart.h>

using namespace irr;

class INotification;
class CMaterialManager;
class CTextureParameters;

/**
 * @class CSegment
 * This class holds all data of the segments that will then be connected
 * by connections
 * @author Christian Keimel / bulletbyte.de
 * @see CConnection
 */
class CSegment : public IRoadPart {
  protected:
    f32 m_fWidth,       /**<! width of the road segment */
        m_fLength,      /**<! length of the road segment */
        m_fBaseOffset;  /**<! height of the road segment */

    bool m_bLevelBase;  /**<! bring all base points to the same Y-value? */

    core::vector3df m_vPosition,    /**<! position of the segment */
                    m_vDirection,   /**<! direction of the segment */
                    m_vNormal,      /**<! normal (upwards) of the segment */
                    m_vPoints[4];   /**<! the 4 points that define the upper polygon of the segment */


    video::IVideoDriver *m_pDrv;    /**<! the videodriver */
    core::stringc m_sName;          /**<! name of the segment */

    core::list<INotification *> m_lNotify;  /**<! list of objects that get notifications on changes of the segment */

    scene::IMeshBuffer *m_pBuffer[6];     /**<! meshbuffers of the segment */
    CTextureParameters *m_pTexParams[6];  /**<! texture parameters of the segment */
    
    /**
     * This method recalculates the meshbuffers and notifies all registered listeners of the modification
     * @see CSegment::m_lNotify
     * @see CSegment::setWidth
     * @see CSegment::setLength
     * @see CSegment::setBaseOffset
     * @see CSegment::setLevelBase
     * @see CSegment::setPosition
     * @see CSegment::setDirection
     * @see CSegment::setNormal
     */
    void attributeChanged();
    
    /**
     * This method recalculates the segment's meshbuffers
     * @see CSegment::CSegment
     * @see CSegment::attributeChanged
     * @see CSegment::load
     * @see CSegment::update
     */
    virtual void recalcMeshBuffer();
    
    /**
     * This method takes an array of vectors and converts them to vertices
     * @param vec the array of vectors
     * @param pTex the texture parameters to be used
     * @param vert the output vertex array
     */
    void fillVertexArray(core::vector3df vec[], CTextureParameters *pTex, video::S3DVertex *vert);

  public:
    /**
     * The construtor. This one is used by the GUI of the editor
     * @param sName name of the segment
     * @param vPosition position of the segment
     * @param pDrv the Irrlicht video driver
     */
    CSegment(core::stringc sName, core::vector3df vPosition, video::IVideoDriver *pDrv);
    /**
     * The construtor. This one is used by the road loading routine
     * @param pDrv the Irrlicht video driver
     */
    CSegment(video::IVideoDriver *pDrv);
    
    /**
     * The destructor
     */
    virtual ~CSegment();

    void setWidth (f32 f);  /**<! set the width of the segment */
    void setLength(f32 f);  /**<! set the length of the segment */

    f32 getWidth ();  /**<! get the width of the segment */
    f32 getLength();  /**<! get the length of the segment */

    void setBaseOffset(f32 f);  /**<! set the base offset of the segment */
    f32 getBaseOffset();        /**<! get the base offset of the segment */

    void setLevelBase(bool b);  /**<! set the base level flag of the segment */
    bool getLevelBase();        /**<! get the base level flag of the segment */

    void setPosition (core::vector3df v); /**<! set the position of the segment */
    void setDirection(core::vector3df v); /**<! set direction of the segment */
    void setNormal   (core::vector3df v); /**<! set normal of the segment */

    const core::vector3df &getPosition ();  /**<! get the position of the segment */
    const core::vector3df &getDirection();  /**<! get the direction of the segment */
    const core::vector3df &getNormal   ();  /**<! get the normal of the segment */

    void setName(const core::stringc &sName); /**<! set the name of the segment */

    const core::stringc &getName();   /**<! get the name of the segment */

    virtual void render();  /**<! render the segment (for editor use only) */

    const core::vector3df &getPoint(u32 i);   /**<! get one of the upper points of the segment */

    virtual void save(io::IAttributes *out);  /**<! save the segment */
    virtual void load(io::IAttributes *in);   /**<! load the segment */

    void addNotify(INotification *p);     /**<! add a notifier that will be notified on changes of the segment */
    void delNotify(INotification *p);     /**<! remove a notifier */

    void update();  /**<! update the meshbuffers */

    virtual CTextureParameters *getTextureParameters(u32 i);  /**<! get one of the texture parameter objects */
    
    virtual u32 getTextureCount() { return 6; }
    
    virtual scene::IMeshBuffer *getMeshBuffer(u32 i);   /**<! get one of the meshbuffers */
};

#endif