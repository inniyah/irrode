#ifndef _C_IRR_ODE_HEIGHTFIELD
  #define _C_IRR_ODE_HEIGHTFIELD

  #include <geom/CIrrOdeGeom.h>

namespace irr {
namespace ode {

const int IRR_ODE_GEOM_HEIGHTFIELD_ID=MAKE_IRR_ID('i','o','g','h');
const wchar_t IRR_ODE_GEOM_HEIGHTFIELD_NAME[0xFF]=L"CIrrOdeGeomHeightfield";

class CIrrOdeGeomHeightfield : public CIrrOdeGeom {
  protected:
    u32 m_iDataId;
    f32 m_fWidth,m_fDepth,m_fThickness;
    u32 m_iWidthSamples,m_iDepthSamples;

  public:
    CIrrOdeGeomHeightfield(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                       const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		                   const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		                   const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeGeomHeightfield();

    static f32 getHeightCallback(void *data, s32 x, s32 z);

    f32 getHeight(s32 x, s32 z);

    virtual void render();
    virtual void OnRegisterSceneNode();
    virtual void initPhysics();

    virtual irr::scene::ESCENE_NODE_TYPE getType() const;
    virtual s32 getID() const;
    virtual const wchar_t *getTypeName();

    virtual void setMassTotal(f32 fMass);

    void setWidth(f32 fWidth);
    void setDepth(f32 fDepth);
    void setThickness(f32 fThickness);
    void setWidthSamples(u32 iWidthSamples);
    void setDepthSamples(u32 iDepthSamples);

  	virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
  	virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

    u32 getWidthSamples();
    u32 getDepthSamples();
};

} //namespace ode
} //namespace irr

#endif
