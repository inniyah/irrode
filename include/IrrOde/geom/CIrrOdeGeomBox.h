#ifndef _C_IRR_ODE_BOX
  #define _C_IRR_ODE_BOX

  #include <geom/CIrrOdeGeom.h>

namespace irr {
namespace ode {

const int IRR_ODE_GEOM_BOX_ID=MAKE_IRR_ID('i','o','g','b');
const wchar_t IRR_ODE_GEOM_BOX_NAME[0xFF]=L"CIrrOdeGeomBox";

class CIrrOdeGeom;

/**
 * @class CIrrOdeGeomBox
 * This is the wrapper class for the ODE box geom
 */
class CIrrOdeGeomBox : public CIrrOdeGeom {
  protected:
    f32 m_fWidth ,  /**< width of the box */
        m_fHeight,  /**< height of the box */
        m_fDepth;   /**< depth of the box */

	bool m_bUseAllMeshBuffers;		/**< use all mesh buffers for collision box? */
	irr::core::array<bool> m_aUseMeshBuffer;	/**< which mesh buffers to use if not all */

  public:
    /**
     * Standard Irrlicht scenenode constructor
     */
    CIrrOdeGeomBox(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                   const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		               const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		               const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    /**
     * Destructor
     */
    virtual ~CIrrOdeGeomBox();

    virtual void render();
    virtual void OnRegisterSceneNode();
    virtual void initPhysics();

    virtual s32 getID() const;
    virtual irr::scene::ESCENE_NODE_TYPE getType() const;
    virtual const wchar_t *getTypeName();

    /**
     * set the total mass of the box
     * @param fMass mass of the box
     */
    virtual void setMassTotal(f32 fMass);

    void setWidth(f32 fWidth);
    void setHeight(f32 fHeight);
    void setDepht(f32 fDepth);

  	virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
  	virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);
};

} //namespace ode
} //namespace irr

#endif
