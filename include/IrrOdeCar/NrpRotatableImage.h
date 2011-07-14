/*
Original file by Dalerank.

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include <IGUIElement.h>
#include <SMeshBuffer.h>

namespace irr
{

namespace video
{
	class ITexture;
}

namespace gui
{

class CNrpRotatableImage : public IGUIElement
{ 
public:
	CNrpRotatableImage(const core::rect<s32>& rectange, IGUIEnvironment* env, s32 id, IGUIElement* parent );

	//! render the compass
	virtual void draw();

	//! set body of compass texture
	virtual void SetTexture( video::ITexture* texture);

	virtual f32 getRotate()
	{
		return matrix_.getRotationDegrees().Z;
	}

	bool HasTexture();

	//! set compass heading
	virtual void SetRotate(const f32 deg)
	{
		matrix_.makeIdentity();
		matrix_.setRotationDegrees(core::vector3df(0.0f, 0.0f, deg));		
	}

protected:

	scene::SMeshBuffer bodyMesh_;

	core::matrix4 matrix_;

	//! sets up a quad mesh of given dimension
	virtual void SetupQuadMesh(scene::SMeshBuffer& mesh, const f32 width);

};

}//namespace gui

}//namespace irr
