#include <irrlicht.h>

namespace irr {
namespace ode {

#ifndef _C_IRR_ODE_SCENE_NODE_FACTORY
  #define _C_IRR_ODE_SCENE_NODE_FACTORY

class CIrrOdeSceneNodeFactory : public irr::scene::ISceneNodeFactory {
  private:
    irr::core::array<irr::core::stringc> m_cNames;
    irr::core::array<irr::scene::ESCENE_NODE_TYPE> m_cTypes;
    irr::scene::ISceneManager *m_pManager;

  public:
    CIrrOdeSceneNodeFactory(irr::scene::ISceneManager *pManager);
    virtual irr::scene::ISceneNode *addSceneNode (irr::scene::ESCENE_NODE_TYPE type, irr::scene::ISceneNode *parent=0);
    virtual irr::scene::ISceneNode *addSceneNode (const c8 *typeName, irr::scene::ISceneNode *parent=0);
    virtual u32 getCreatableSceneNodeTypeCount() const;
    virtual irr::scene::ESCENE_NODE_TYPE getCreateableSceneNodeType (u32 idx) const;
    virtual const c8 *getCreateableSceneNodeTypeName(irr::scene::ESCENE_NODE_TYPE type) const;
    virtual const c8 *getCreateableSceneNodeTypeName(u32 idx) const;
    virtual ~CIrrOdeSceneNodeFactory ();
};

} //namespace ode
} //namespace irr

#endif

