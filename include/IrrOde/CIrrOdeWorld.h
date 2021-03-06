#ifndef _C_IRR_ODE_WORLD
  #define _C_IRR_ODE_WORLD

  #include <CIrrOdeDampable.h>
  #include <IIrrOdeContactParameters.h>

  #define MAX_CONTACTS 16

namespace irr {
namespace ode {

const int IRR_ODE_WORLD_ID=MAKE_IRR_ID('i','o','w','d');
const wchar_t IRR_ODE_WORLD_NAME[0xFF]=L"CIrrOdeWorld";

class CIrrOdeSpace;
class CIrrOdeBody;
class CIrrOdeGeom;
class IIrrOdeStepMotor;
class IIrrOdeEventWriter;
class CIrrOdeEventBodyMoved;

class CIrrOdeWorld : public CIrrOdeDampable {
  protected:
    irr::u32 m_iJointGroupId;
    irr::u32 m_iWorldId;
    irr::f32 m_fStepSize;      /**< the stepsize */
    irr::s32 m_iNodesInitialized;    /**< the number of nodes that have already been initialized */

    bool m_bDrawEditorMesh;

    CIrrOdeSpace *m_pWorldSpace;

    irr::core::vector3df m_cGravity;
    irr::core::list<CIrrOdeBody *> m_pBodies;
    irr::core::list<CIrrOdeGeom *> m_pGeoms;
    irr::core::list<CIrrOdeSpace *> m_pSpaces;
    #ifdef _IRREDIT_PLUGIN
      irr::core::list<CIrrOdeSceneNode *> m_pChildNodes;
    #endif

    irr::core::stringw m_sSurfaceFile;

    irr::core::list<irr::ode::CIrrOdeSceneNode *> m_pSceneNodes;     /**< the scene nodes */
    irr::core::list<irr::ode::IIrrOdeStepMotor *> m_lStepMotors;     /**< all step motors */

    /**
     * This is a irr::core::list of all objects that want to post events
     * about their state change after the step has been done.
     */
    irr::core::list<irr::ode::IIrrOdeEventWriter *> m_lChanged;

    void loadParameter(irr::io::IXMLReader *pReader);
    u32 loadFromFile(const wchar_t *sName);
    void updateSurfaceParameterList();

    bool isRegisteredOdeSceneNode(irr::scene::ISceneNode *pNode);

  public:
    CIrrOdeWorld(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                 const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		             const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		             const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeWorld();

    void frameUpdate();
    u32 getWorldId();

    void addBody(CIrrOdeBody *pBody);
    void addGeom(CIrrOdeGeom *pGeom);
    void addSpace(CIrrOdeSpace *pSpace);
    void removeBody(CIrrOdeBody *pBody);
    void removeGeom(CIrrOdeGeom *pGeom);
    void removeSpace(CIrrOdeSpace *pSpace);

    u32 getBodyCount();
    u32 getGeomCount();

    virtual void setLinearDamping(f32 fScale);
    virtual f32 getLinearDamping();
    virtual void setAngularDamping(f32 fScale);
    virtual f32 getAngularDamping();
    virtual void setLinearDampingThreshold(f32 fThreshold);
    virtual f32 getLinearDampingThreshold();
    virtual void setAngularDampingThreshold(f32 fThreshold);
    virtual f32 getAngularDampingThreshold();
    virtual void setMaxAngularSpeed(f32 fMaxSpeed);
    virtual f32 getMaxAngularSpeed();

    virtual void setAutoDisableFlag(int iFlag);
    virtual int getAutoDisableFlag();
    virtual void setAutoDisableLinearThreshold(f32 fThreshold);
    virtual f32 getAutoDisableLinearThreshold();
    virtual void setAutoDisableAngularThreshold(f32 fThreshold);
    virtual f32 getAutoDisableAngularThreshold();
    virtual void setAutoDisableSteps(int iSteps);
    virtual int getAutoDisableSteps();
    virtual void setAutoDisableTime(f32 fTime);
    virtual f32 getAutoDisableTime();

    virtual void initPhysics();

    void stopPhysics();

    void setStepSize(f32 fStepSize);
    f32 getStepSize();

    void step(f32 fTime);

    void setGravity(irr::core::vector3df cGravity);
    irr::core::vector3df &getGravity();
    void setERP(f32 fERP);
    f32 getERP();
    void setCFM(f32 CFM);
    f32 getCFM();

    s32 getID();

    virtual void render();
    virtual void OnRegisterSceneNode();

    irr::scene::ESCENE_NODE_TYPE getType() const;
    virtual const wchar_t *getTypeName();

    u32 getJointGroupId();

    irr::core::list<CIrrOdeBody *> &getBodyList();

    CIrrOdeSpace *getSpace();
    void setSpace(CIrrOdeSpace *pWorldSpace);
    void setContactCalculator(IIrrOdeContactParameters *pCalculator);
    IIrrOdeContactParameters *getContactCalculator();

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

  	virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
    virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);
    virtual void removeFromPhysics();

    #ifdef _IRREDIT_PLUGIN
      void addOdeChildNode(CIrrOdeSceneNode *pNode);
      void removeOdeChildNode(CIrrOdeSceneNode *pNode);
    #endif

    /**
     * load a surface parameter XML file created with IrrOdeSurfaceEdit
     * @param sName the file to load
     * @return number of surface parameters loaded
     */
    u32 setSurfaceXml(const wchar_t *sName);

    /**
     * This method resets the body collision attributes. It's called by the device before each step
     */
    void resetCollisionAttributes();

    /**
     * This method is called when an event is posted
     * @param pEvent the posted event
     */
    virtual bool onEvent(IIrrOdeEvent *pEvent) { return false; }

    /**
     * This method is called to see whether or not an event is handled by this irr::core::listener
     * @param pEvent the event in question
     * @return "true" if the irr::core::listener handles the event, "false" otherwise
     */
    virtual bool handlesEvent(IIrrOdeEvent *pEvent) { return false; }

		void addStepMotor(IIrrOdeStepMotor *pMotor);    /*!< add a step motor */
		void removeStepMotor(IIrrOdeStepMotor *pMotor); /*!< remove a step motor */
		void stepStepMotors();                          /*!< this method is called from the IrrOdeDevice to step the step motors */

    void removeEventWriter(IIrrOdeEventWriter *p);
    void objectChanged(IIrrOdeEventWriter *p);

    void addOdeSceneNode(CIrrOdeSceneNode *pNode);    /*!< add an ode scenenode to the manager */
    void removeOdeSceneNode(CIrrOdeSceneNode *pNode); /*!< remove an ode scenenode from the manager */

    irr::core::list<CIrrOdeSceneNode *> &getIrrOdeNodes();						/*!< get a irr::core::list of all registered IrrOde scene nodes */
		void sceneNodeInitialized(CIrrOdeSceneNode *pNode);               /*!< callback function to trace initialization */

		/**
		 * This is the actual clone method
     * @param pSource the node to be cloned
     * @param newParent the parent the clone will be attached to
     * @param newSmgr the new scene manager
     * @param iNewId the new ID of the node ("-1" (default) will auto generate new id)
		 */
    irr::scene::ISceneNode *cloneOdeNode(irr::scene::ISceneNode *pSource, irr::scene::ISceneNode *newParent, irr::scene::ISceneManager *newSmgr, s32 iNewId=-1);

		/**
		 * This methdo is used to clone a complete (sub)tree with ODE scene nodes in (can as well clone any other type of node. Actually this method
     * doesn't do any cloning, it posts an IrrOde event and call CIrrOdeManager::cloneOdeNode
     * @param pSource the node to be cloned
     * @param newParent the parent the clone will be attached to
     * @param newSmgr the new scene manager
     * @see cloneOdeNode
     */
    irr::scene::ISceneNode *cloneTree(irr::scene::ISceneNode *pSource, irr::scene::ISceneNode *newParent, irr::scene::ISceneManager *newSmgr);

    /**
     * This method removes all IrrOde objects of a (sub)tree from physics. Note that pNode will not be
     * removed, this method is called from CIrrOdeBody::removeFromPhysics
     * @param pNode the root node of the tree to be removed
     * @see CIrrOdeBody::removeFromPhysics
     */
    void removeTreeFromPhysics(irr::scene::ISceneNode *pNode);

    /**
     * Remove a node from the scene. This method sends a CIrrOdeEventNodeRemoved event
     * @param pNode node to remove
     * @see CIrrOdeEventNodeRemoved
     */
    void removeSceneNode(irr::scene::ISceneNode *pNode);
};

} //namespace ode
} //namespace irr

#endif
