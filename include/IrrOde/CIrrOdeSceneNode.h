#ifndef _C_IRR_ODE_SCENENODE
  #define _C_IRR_ODE_SCENENODE

  #include <irrlicht.h>
  #include <event/IIrrOdeEventListener.h>

/**
 * some defines to display debug data:
 *
 * _TRACE_CONSTRUCTOR_DESTRUCTOR: plot constructor/destructor information to stdout
 * _TRACE_INIT_PHYSICS: plot init physics information to stdout
 * _DRAW_BOUNDING_BOXES: draw bounding boxes of the affected scene nodes
 * _DRAW_JOINT_INFO: draw joint info lines
 * _TRACE_CHANGED_PROPERTIES: trace changed properties
 * _TRACE_FACTORY: trace scenenode factory
 * _IRREDIT_PLUGIN: display additional objects for the IrrEdit plugin
 */

namespace irr {
namespace ode {

class CIrrOdeManager;
class IIrrOdeDevice;
class CIrrOdeWorld;

/**
 * @class CIrrOdeSceneNode
 * This class is the base class for all scene nodes provided by IrrODE
 */
class CIrrOdeSceneNode : public irr::scene::ISceneNode, public ode::IIrrOdeEventListener {
  protected:
    irr::core::aabbox3df m_cBoundingBox;               /**< the bounding box of the scene node */
    irr::scene::ISceneManager *m_pSceneManager;         /**< the scene manager */
    irr::video::IVideoDriver *m_pVideoDriver;           /**< the video driver */
    bool m_bPhysicsInitialized;             /**< is the physics of this node alread initialized? */
    irr::core::stringc m_sOdeClassName;                /**< this node's irrODE classname */
    s32 m_iIdent;                           /**< this node's irrODE ID */
    CIrrOdeManager *m_pOdeManager;          /**< the ODE manager */
    IIrrOdeDevice *m_pOdeDevice;            /**< the ODE device */
    CIrrOdeWorld *m_pWorld;                 /**< world of this node */

    irr::video::SMaterial m_cMat;                /**< the material of the node */
    irr::scene::ITriangleSelector *m_pSelector;

    void *m_pUserData;

    irr::scene::ISceneNode *getAncestorOfType(irr::scene::ESCENE_NODE_TYPE iType);    /**< retrieve an ancestor of a given type */

    irr::scene::ISceneNode *findFirstNodeOfType(irr::scene::ESCENE_NODE_TYPE iType, irr::scene::ISceneNode *pNode);  /**< find the first node of a given type */

    #ifdef _IRREDIT_PLUGIN
      irr::scene::IAnimatedMesh *m_pMesh;   /**< the mesh to be renderer in IrrEdit when compiled as plugin */
  	  c8 m_sResources[1024];    /**> the path name of the resources when copiled as plugin */
  	  bool m_bDrawEditorMesh;
    #endif

    void cloneChildren(irr::scene::ISceneNode *pNewParent, irr::scene::ISceneManager *pNewManager);
  public:
    CIrrOdeSceneNode(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                     const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		                 const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		                 const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeSceneNode();

    virtual void initPhysics(); /*!< init the dynamic object. Implemented by subclasses */
    virtual const irr::core::aabbox3df &getBoundingBox() const;

    /**
     * Get the type name of the node
     * @return the type name of the node
     */
    virtual const wchar_t *getTypeName()=0;

    bool physicsInitialized();

    static const c8 *nodeNameToC8(const wchar_t *sName); /*!< a little helper that converts wchar_t* to c8* */

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

    irr::core::stringc getOdeClassname();                /*!< set the IrrODE classname */
    void setOdeClassname(irr::core::stringc sClassName); /*!< get the IrrODE classname */

    int getIdent();             /*!< get the IrrODE identifier */
    void setIdent(int iIdent);  /*!< set the IrrODE identifier */

    virtual u32 getMaterialCount();
    virtual irr::video::SMaterial &getMaterial(u32 iIdx);
    virtual void 	addChild (irr::scene::ISceneNode *child);

    virtual void render();
    virtual void removeFromPhysics();  /**< remove the node from physics */
    virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true); /**< copy the parameter to another ODE scene node */

    virtual void remove();

    irr::scene::ISceneManager *getSceneManager();

    irr::scene::ITriangleSelector *getTriangleSelector();

    void setUserData(void *p);
    void *getUserData();

    void setPhysicsInitialized(bool b) { m_bPhysicsInitialized=b; }
    #ifdef _IRREDIT_PLUGIN
      void setDrawEditorMesh(bool b);
    #endif

    virtual void setParent(irr::scene::ISceneNode *newParent);

    irr::scene::ISceneNode *getChildByName(const c8 *sName, irr::scene::ISceneNode *pParent);
};

} //namespace ode
} //namespace irr

#endif
