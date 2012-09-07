#ifndef _C_IRR_ODE_BODY
  #define _C_IRR_ODE_BODY

  #include <CIrrOdeDampable.h>
  #include <event/IIrrOdeEventWriter.h>

namespace irr {
namespace ode {

const int IRR_ODE_BODY_ID=MAKE_IRR_ID('i','o','b','d'); /**< create an Irrlicht ID for this class */
const wchar_t IRR_ODE_BODY_NAME[0xFF]=L"CIrrOdeBody";   /**< store a name for this class */

class CIrrOdeWorld;       /**< foreward declaration of CIrrOdeWorld      */
class CIrrOdeJoint;       /**< foreward declaration of CIrrOdeJoint      */
class CIrrOdeSpace;       /**< foreward declaration of CIrrOdeSpace      */
class CIrrOdeGeom;        /**< foreward declaration of CIrrOdeGeom       */
class CIrrOdeGeomRay;     /**< foreward declaration of CIrrOdeGeomRay    */
class IIrrOdeStepMotor;   /**< foreware declaration of IIrrOdeStepMotor  */
class IIrrOdeEventQueue;  /**< foreward declaration of IIrrOdeEventQueue */
class IIrrOdeMotor;       /**< foreward declaration of IIrrOdeMotor      */

/**
 * @class CIrrOdeBody
 * This class encapsulates the ODE body. Bodies are dynamic objects in ODE.
 */
class CIrrOdeBody : public CIrrOdeDampable, public IIrrOdeEventWriter {
  protected:
    u32 m_iBodyId;                          /**< this body's ODE dBodyID */
    u32 m_iMass;                            /**< this body's ODE dMass */
    CIrrOdeWorld *m_pWorld;                 /**< this body's world */
    CIrrOdeBody *m_pParentBody;             /**< this body's parent body */
    irr::core::list<CIrrOdeGeom *> m_pGeoms;           /**< this body's geoms */
    irr::core::list<CIrrOdeJoint *> m_pJoints;         /**< this body's joints */
    irr::core::list<CIrrOdeBody *> m_pChildBodies;     /**< this body's child bodies connected by joints */
    irr::core::list<IIrrOdeStepMotor *> m_lStepMotors; /**< irr::core::list of all step motors attached to the body */
    irr::core::list<IIrrOdeMotor *> m_lMotors;         /**< irr::core::list of all motors and servos attached to the body */
    bool m_bEnabled,                        /**< is this body active or not? */
         m_bParamMaster,                    /**< is this body the parameter master, i.e. does it set the body parameters of other bodies with the same ODE classname? */
         m_bCollision,                      /**< did this body collide in the last simulation step? */
         m_bDampingChanged,                 /**< was one of the damping parameters changed since the last simulation step? */
         m_bFastMoving,                     /**< is this a fast moving body? */
				 m_bUpdateGraphics,                 /**< is the graphical representation of this ODE body updated? */
         m_bFiniteRotationMode;             /**< the finite rotation mode flag */
    s32 m_iGravityMode,                     /**< the gravity mode */
        m_iCollisionMaterial;               /**< index of the material if the body collided */
    CIrrOdeGeom *m_pTouching;               /**< the geom the body is touching */
    CIrrOdeGeomRay *m_pRay;                 /**< the ray for a fast moving object */

    IIrrOdeEventQueue *m_pQueue;            /**< the event queue used to post all kinds of events */

    irr::core::vector3df m_vLinear,              /**< linear velocity of the body. Is updated when calling "getLinearVelocity" */
              m_vAngular,             /**< angular velocity of the body. Is updated when calling "getAngularVelocity" */
              m_vFiniteRotationAxis,  /**< the finite rotation mode axis */
              m_vCollisionPoint;      /**< point of the collision if the body collided */

  public:
    CIrrOdeBody(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		            const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		            const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeBody();

    /**
     * get the body's ODE ID
     * @return the body's dBodyID
     */
    u32 getBodyId();

    virtual void addChild(irr::scene::ISceneNode *pChild);

    virtual void setLinearDamping(f32 fScale);                /**< set the linear damping of the body */
    virtual f32 getLinearDamping();                           /**< get the linear damping of the body */
    virtual void setAngularDamping(f32 fScale);               /**< set the angular damping of the body */
    virtual f32 getAngularDamping();                          /**< get the angular damping of the body */
    virtual void setLinearDampingThreshold(f32 fThreshold);   /**< set the linear damping threshold of the body */
    virtual f32 getLinearDampingThreshold();                  /**< get the linear damping threshold of the body */
    virtual void setAngularDampingThreshold(f32 fThreshold);  /**< set the angular damping threshold of the body */
    virtual f32 getAngularDampingThreshold();                 /**< get the angular damping threshold of the body */
    virtual void setMaxAngularSpeed(f32 fMaxSpeed);           /**< set the maximum angular speed of the body */
    virtual f32 getMaxAngularSpeed();                         /**< get the maximum angular speed of the body */

    virtual void setAutoDisableFlag(int iFlag);                  /**< enable / disable the auto disable feature */
    virtual int getAutoDisableFlag();                            /**< is the auto disable feature enabled? */
    virtual void setAutoDisableLinearThreshold(f32 fThreshold);  /**< set auto disable linear threshold */
    virtual f32 getAutoDisableLinearThreshold();                 /**< get auto disable linear threshold */
    virtual void setAutoDisableAngularThreshold(f32 fThreshold); /**< set auto disable angular threshold */
    virtual f32 getAutoDisableAngularThreshold();                /**< get auto disable angular threshold */
    virtual void setAutoDisableSteps(int iSteps);                /**< set auto disable steps */
    virtual int getAutoDisableSteps();                           /**< get auto disable steps */
    virtual void setAutoDisableTime(f32 fTime);                  /**< set auto disable time */
    virtual f32 getAutoDisableTime();                            /**< get auto disable time */

    virtual void initPhysics();  /**< init the body's physics */

    void setFiniteRotationMode(bool bFlag);        /**< set finite rotation mode */
    bool getFiniteRotationMode();                  /**< get finite rotation mode */
    void setFiniteRotationAxis(irr::core::vector3df &pAxis);  /**< set finite rotation axis */
    irr::core::vector3df &getFiniteRotationAxis();            /**< get finite rotation axis */
    CIrrOdeWorld *getWorld();                      /**< get the assiciated CIrrOdeWorld object */
    void setGravityMode(int iMode);                /**< set the gravity mode */
    int getGravityMode();                          /**< get the gravity mode */
    CIrrOdeGeom *getFirstGeom();                   /**< get the body's first geom */
    CIrrOdeGeom *getNextGeom(CIrrOdeGeom *pPrev);  /**< get the body's next geom */
    void addGeom(CIrrOdeGeom *pGeom);              /**< add a geom to the body */
    void addJoint(CIrrOdeJoint *pJoint);           /**< add a joint to the body */

    irr::core::vector3df getPointVel(irr::core::vector3df vPos);         /**< get the velocity at a specific position */
    irr::core::vector3df getRelPointVel(irr::core::vector3df vPos);      /**< get the relative velocity at a specific position */
    irr::core::vector3df getRelPointPos(irr::core::vector3df vPos);      /**< get the relative position of a specific position */

    void removeGeom(CIrrOdeGeom *pGeom);  /**< remove a geom from the body */

    virtual void render();  /**< render the body (for debugging) */
    virtual void OnRegisterSceneNode();

    s32 getID();  /**< get the body's ID */

    irr::scene::ESCENE_NODE_TYPE getType() const;    /**< get the body's object type */
    const char *getTypeName() const;     /**< get the body's object type name */
    bool isEnabled();                    /**< is the body enabled? */
    void frameUpdate();                  /**< called each frame to check for activation changes */

    void addForce(irr::core::vector3df force);                          /**< add a force to the body */
    void addForceAtPosition(irr::core::vector3df pos, irr::core::vector3df force); /**< add a force at a specific position to the body */
    void addTorque(irr::core::vector3df torque);                        /**< add a torque to the body */
    void setTorque(irr::core::vector3df torque);                        /**< set the body's torque */

    virtual void setPosition(const irr::core::vector3df &newPos);  /**< set the body's position */
    virtual void setRotation(const irr::core::vector3df &newRot);  /**< set the body's rotation */

    CIrrOdeBody *getParentBody();  /**< get the parent body */

    virtual const wchar_t *getTypeName();  /**< get the object type name */

    void addChildBody(CIrrOdeBody *pBody);  /**< add a child body */

    irr::core::list<CIrrOdeJoint *> getJoints();  /**< get a irr::core::list of the body's joints */

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

    virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);

    void setParamMaster(bool b);  /**< set this body to be the parameter master for all bodies of the same OdeClassName */
    bool isParamMaster();         /**< is this body the parameter master? */

    virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true); /**< copy the parameters to another IrrOdeSceneNode */
    virtual void removeFromPhysics(); /**< remove the body from physics */
    void doRemoveFromPhysics();

    void addMass(u32 iMassId);        /**< add a geom's mass to the body */
    void setEnabled(bool bEnabled);   /**< enable or disable the body */

    void setCollision(bool b);
    bool getCollision();

    void setIsTouching(CIrrOdeGeom *pTouch);
    CIrrOdeGeom *getTouched();

    virtual void setLinearVelocity(irr::core::vector3df newVel);     /**< update the node's linear velocity wihout affecing ODE */
    virtual void setAngularVelocity(irr::core::vector3df newVel);    /**< update the node's angular velocity without affecting ODE */

    virtual irr::core::vector3df getLinearVelocity();    /**< get the node's "linear velocity" attribute wihtout querying ODE */
    virtual irr::core::vector3df getAngularVelocity();   /**< get the node's "angular velocity" attribute wihtout querying ODE */

    void setNodeLinearDamping(f32 fScale);  /**< set the node's "linear damping" attribute without affecting ODE */
    void setNodeAngularDamping(f32 fScale); /**< set the node's "angular damping" attribute without affecting ODE */

    f32 getNodeLinearDamping();
    f32 getNodeAngularDamping();

    void setCollisionPoint(irr::core::vector3df vPoint);   /**< set the point of the collision */
    irr::core::vector3df getCollisionPoint();              /**< get the point of the collision */

    void setCollisionMaterial(s32 iIdx);    /**< set the index of the collision material */
    s32 getCollisionMaterial();             /**< get the index of the collision material */

    /**
     * This method shows whether or not the damping has changed since the last step.
     * Useful for the (upcoming) IrrOdeNet and IrrOdeRecorder
     * @return has the damping changed since the last step
     */
    bool dampingChanged();

    /**
     * Use this method to mark a body as fast moving, e.g. bullets. If a body is marked as "fast moving"
     * a ray will be cast after each step from the old position to the new one so that a detection is done
     * to find out whether or not the body has passed another body in the last step
     * @param b fast moving flag
     */

    void setIsFastMoving(bool b);
    /**
     * query the fast moving flag
     * @return the fast moving flag
     */
    bool isFastMoving();

    /**
     * This is a callback method that is used for the "fast moving" ray. In this method the ray's position
     * and target are updated
     */
    void bodyMoved(irr::core::vector3df newPos);

    /**
     * add a step motor
     * @param p the step motor to add
     */
    void addStepMotor(IIrrOdeStepMotor *p);

    /**
     * add a motor or servo
     * @param p the motor/servo to add
     */
    void addMotor(IIrrOdeMotor *p);

    /**
     * remove a step motor
     * @param p the step motor to remove
     */
    void removeStepMotor(IIrrOdeStepMotor *p);

    void setOdeRotation(irr::core::vector3df vRot);

    irr::core::vector3df getPosRelPoint(irr::core::vector3df vPos);
    irr::core::vector3df getVectorFromWorld(irr::core::vector3df vPos);

    void getOdeRotation(irr::core::vector3df &cRot);
    void getOdePosition(irr::core::vector3df &cPos);

    /**
     * Is the graphical representation of this body updated when the physics body moves? Can be false
     * for "dummy" bodies that are used if more than joint joints connect two bodies (e.g. hinge and slider
     * joints to attach a wheel to a car
     */
		bool updateGraphics() { return m_bUpdateGraphics; }

		/**
		 * Set this value to "false" if you don't want to update the graphical representation of an ODE body.
		 * Parameter can't be changed after "initPhysics" was called.
		 */
		void setUpdateGraphics(bool b) { if (!m_bPhysicsInitialized) m_bUpdateGraphics=b; }

		/**
		 * Get a irr::core::list of all child bodies attached to this one
		 * @return irr::core::list of all child bodies attached to this one
		 */
    irr::core::list<CIrrOdeBody *> &getChildBodies();

    CIrrOdeGeom *getGeomFromName(const   c8 *sName);
    CIrrOdeJoint *getJointFromName(const c8 *sName);
    CIrrOdeBody *getChildBodyFromName(const c8 *sName);
    IIrrOdeStepMotor *getStepMotorFromName(const c8 *sName);
    IIrrOdeMotor *getMotorFromName(const c8 *sName);

    /**
     * This method is called when an event is posted
     * @param pEvent the posted event
     */
    virtual bool onEvent(IIrrOdeEvent *pEvent);

    /**
     * This method is called to see whether or not an event is handled by this irr::core::listener
     * @param pEvent the event in question
     * @return "true" if the irr::core::listener handles the event, "false" otherwise
     */
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    /**
     * The implementation of the "writeEvent" method of the "IIrrOdeEventWriter" interface.
     * @return NULL, because the ODE object's events will be writter by the IrrOdeDevice.
     * @see IIrrOdeEventWriter::writeEvent
     */
    virtual IIrrOdeEvent *writeEvent() { return NULL; }

    /**
     * This implementation shows that we are a body to the IrrOdeDevice's
     * event creation method
     */
    virtual eEventWriterType getEventWriterType() {
      return eIrrOdeEventWriterBody;
    }
};

} //namespace ode
} //namespace irr

#endif
