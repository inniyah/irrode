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
class CIrrOdeEventBodyMoved;

class CIrrOdeWorld : public CIrrOdeDampable {
  protected:
    u32 m_iJointGroupId;
    u32 m_iWorldId;
    f32 m_fStepSize;      /**< the stepsize */

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

	void loadParameter(irr::io::IXMLReader *pReader);
	u32 loadFromFile(const wchar_t *sName);
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

    void setStepSize(f32 fStepSize);
    f32 getStepSize();
    void setQuickStepNumIterations(s32 iIterations);
    s32 getQuickStepNumIterations();
    void setQuickStepSOR(f32 fSOR);
    f32 getQuickStepSOR();

    void step(f32 fTime);
    void quickStep(f32 fTime);

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
};

} //namespace ode
} //namespace irr

#endif
