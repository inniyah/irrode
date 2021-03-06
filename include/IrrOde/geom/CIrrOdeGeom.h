#ifndef _C_IRR_ODE_GEOM
  #define _C_IRR_ODE_GEOM

  #include <CIrrOdeSceneNode.h>
  #include <CIrrOdeSurfaceParameters.h>

namespace irr {
namespace ode {

const double GRAD_PI =180.0 / 3.1415926535897932384626433832795;
const double GRAD_PI2=3.1415926535897932384626433832795 / 180.0;

const int IRR_ODE_GEOM_ID=MAKE_IRR_ID('i','o','g','m');
const wchar_t IRR_ODE_GEOM_NAME[0xFF]=L"CIrrOdeGeom";

class CIrrOdeBody;
class CIrrOdeWorld;
class CIrrOdeSpace;

/**
 * @class CIrrOdeGeom
 * This is the base class for all ODE geoms (like sphere, box and trimesh).
 */
class CIrrOdeGeom : public CIrrOdeSceneNode {
  protected:
    u32 m_iGeomId,                /**< the ODE geom id */
        m_iTriggerId;             /**< id for the trigger events (if this geom is a trigger) */
    u32 m_iMass,                  /**< the ODE mass    */
			  m_iCollisionGroup;				/**< the collision group. Two geoms with the same group!=0 won't collide */
    f32 m_fMass;                  /**< the mass */
    bool m_bCollide,              /**< does this geom collide with other geoms? */
         m_bSurfaceMaster,
         m_bTrigger;              /**< does this geom send trigger events when colliding with a body? */
    CIrrOdeBody *m_pBody;         /**< the IrrODE body the geom is attached to */
    CIrrOdeWorld *m_pWorld;       /**< the IrrOde world the geom is in */
    CIrrOdeSpace *m_pSpace;       /**< the IrrOde space the geom is in */
    irr::core::vector3df m_cMassTranslate;   /**< the translation of the geom's mass */
    irr::core::vector3df m_cCenterOfGravity; /**< the geom's center of gravity    */
    irr::core::vector3df m_cInertia1;
    irr::core::vector3df m_cInertia2;
    irr::core::vector3df m_cOffsetPos;
    irr::core::vector3df m_cOffsetRot;

    irr::core::array<CIrrOdeSurfaceParameters *> m_aParams;
    irr::core::array<irr::core::stringc> m_aParamNames;

  public:
    /**
     * The constructor (standard irrlicht scenenode)
     * @param parent the parent node
     * @param mgr the irr::scene::ISceneManager
     * @param id the ID of the node
     * @param position the position of the node
     * @param rotation the rotation of the node
     * @param scale the scale of the node
     */
    CIrrOdeGeom(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		            const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		            const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    /** the destructor */
    virtual ~CIrrOdeGeom();

    /**
     * get the body the geom is attached to
     * @return the IrrOdeBody the geom is attached to, NULL if the geom is static (i.e. not attached to a body)
     */
    CIrrOdeBody *getBody();

    /**
     * Attach the geom to a body
     * @param pBody the body the geom will be attached to
     */
    void setBody(CIrrOdeBody *pBody);

    /**
     * Set the total mass of the geom
     * @param fMass the total mass of the geom
     */
    virtual void setMassTotal(f32 fMass)=0;

    /**
     * Get the geom's ODE dGeomID. The parameter is only necessary for static trimeshes which can contain more
     * than one ODE trimesh.
     * @param iNum the index of the geom id to return
     * @return the geom's ODE dGeomID
     */
    virtual u32 getGeomId(u32 iNum=0);

    /**
     * Get the Geom's type name
     * @return the geom's type name
     */
    virtual const wchar_t *getTypeName()=0;

    /**
     * get the number of surface parameters
     * @return the number of surface parameters
     */
    u32 getSurfaceParametersCount() const;

    /**
     * get the geom's surface parameters
     * @param iIdx only used for CIrrOdeGeomTrimesh, because it is the only geom that can have more than one
     */
    virtual CIrrOdeSurfaceParameters *getSurfaceParameters(u32 iIdx);

    /**
     * Get the name of a surface parameter
     * @param iIdx index of the requested parameter set
     * @return name of the requested parameter set
     */
    const c8 *getSurfaceParameterName(u32 iIdx);

    /**
     * Set the name of a surface parameter. The parameter will
     * be taken from the irr::core::list loaded by the CIrrOdeWorld object
     * upon physics initialization
     * @param iIdx index of the paramter to set
     * @param s the name of the paramter set
     */
    void setSurfaceParameterName(u32 iIdx, const c8 *s);

    void setSurfaceParameter(u32 iIdx, CIrrOdeSurfaceParameters *p);

    CIrrOdeWorld *getWorld();

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

    /**
     * set the mass parameters
     * @param fMass the new absolute mass
     * @param c
     * @param i1
     * @param i2
     */
    void setMassParameters(f32 fMass, irr::core::vector3df c, irr::core::vector3df i1, irr::core::vector3df i2);

		/**
		 * the "setPosition" method is overridden to make sure the IrrOdeGeom nodes always have a relative position of (0,0,0)
		 * @param newpos the new position. Will be discarded and set to (0,0,0)
		 */
		virtual void setPosition(const irr::core::vector3df &newpos);

    /**
     * Translate the center of gravity to a new position
     * @param pos the new position
     */
    void setMassTranslation(irr::core::vector3df pos);

    virtual void initPhysics();

    virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);
    virtual void removeFromPhysics();

    void setOffsetPosition(irr::core::vector3df pPos);
    void setOffsetQuaternion(irr::core::vector3df pRot);

    /**
     * Set whether or not this geom collides with other geoms
     * @param b the new value
     */
    void setCollide(bool b);

    /**
     * Does this geom collide with other geoms?
     * @return true if this geom collides with other geoms
     */
    bool doesCollide();

    /**
     * Set the m_iCollisionGroup attribute. A value of zero means the geom will collide with any other geoms,
     * if the value is not equal zero it won't collide with other geoms of the same group id
     * @see m_iCollisionGroup
     */
		void setCollisionGroup(u32 i) { m_iCollisionGroup=i; }

		/**
		 * Get the m_iCollisionGroup attribute
		 * @see m_iCollisionGroup
		 */
		u32 getCollisionGroup() { return m_iCollisionGroup; }

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

    /**
     * Is this geom a trigger?
     * return true or false
     */
    bool isTrigger() { return m_bTrigger; }

    /**
     * Set the trigger flag of this geom
     * @param b the new flag
     */
    void setIsTrigger(bool b) { m_bTrigger=b; }

    /**
     * Get the trigger id
     * @return the trigger id
     */
    s32 getTriggerId() { return m_iTriggerId; }

    /**
     * Set the trigger id
     * @param i the trigger id
     */
    void setTriggerId(s32 i) { m_iTriggerId=i; }
};

} //namespace ode
} //namespace irr

#endif
