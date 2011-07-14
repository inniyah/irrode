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

//#include "StdAfx.h"
#include "NrpRotatableImage.h"
#include <IVideoDriver.h>
#include <IGUIEnvironment.h>
#include <ISceneNode.h>
#include <ITexture.h>

namespace irr
{

namespace gui
{

CNrpRotatableImage::CNrpRotatableImage(const core::recti& rectange,
									   IGUIEnvironment* env,
									   s32 id,
									   IGUIElement* parent )
					: IGUIElement( gui::EGUIET_ELEMENT, env, parent, id, rectange )
{
	SetupQuadMesh(bodyMesh_, 1.0f);
}

//! render the compass
void CNrpRotatableImage::draw()
{
	if (!IsVisible) return;

	video::IVideoDriver* driver = Environment->getVideoDriver();

	const core::rect<s32> oldViewPort = driver->getViewPort();				//сохраняем параметры экрана
	driver->setViewPort(AbsoluteRect);										//устанавливаем параметры отображения как нашу область элемента

	const core::matrix4 oldProjMat = driver->getTransform(video::ETS_PROJECTION); //сохраняем текущую проекцию экрана
	driver->setTransform(video::ETS_PROJECTION, core::matrix4());			//ставим пустую проекцию

	const core::matrix4 oldViewMat = driver->getTransform(video::ETS_VIEW);	//сохраняем матрицу отображения
	driver->setTransform(video::ETS_VIEW, core::matrix4());					//ставим пустую матрицу отображения
	driver->setTransform(video::ETS_WORLD, matrix_);						//савим матрицу оторбажения с поворотом на нужный угол

	driver->setMaterial(bodyMesh_.Material);								//устанавливаем материал отрисовки
	driver->drawMeshBuffer(&bodyMesh_);										//рисуем карту

	driver->setTransform(video::ETS_VIEW, oldViewMat);						//возвращаем матрицу вида
	driver->setTransform(video::ETS_PROJECTION, oldProjMat);				//восстанавливаем проекцию
	driver->setViewPort(oldViewPort);										//востанавливаем параметры отображения экрана

	IGUIElement::draw();
}

void CNrpRotatableImage::SetupQuadMesh(scene::SMeshBuffer& mesh, const f32 width)
{
	const f32 halfWidth = width / 1.f;
	mesh.Vertices.set_used(4);
	mesh.Indices.set_used(6);

	const video::SColor white( 0xFF, 0xFF, 0xFF, 0xFF);

	mesh.Vertices[0] = video::S3DVertex(-halfWidth, -halfWidth, 0.f, 0.f, 0.f, 1.f, white, 0.f, 1.f);
	mesh.Vertices[1] = video::S3DVertex(-halfWidth,  halfWidth, 0.f, 0.f, 0.f, 1.f, white, 0.f, 0.f);
	mesh.Vertices[2] = video::S3DVertex( halfWidth,  halfWidth, 0.f, 0.f, 0.f, 1.f, white, 1.f, 0.f);
	mesh.Vertices[3] = video::S3DVertex( halfWidth, -halfWidth, 0.f, 0.f, 0.f, 1.f, white, 1.f, 1.f);

	mesh.Indices[0] = 0;
	mesh.Indices[1] = 1;
	mesh.Indices[2] = 2;
	mesh.Indices[3] = 2;
	mesh.Indices[4] = 3;
	mesh.Indices[5] = 0;

	mesh.getMaterial().Lighting = false;
	mesh.getMaterial().MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
}

//! set body of compass texture
void CNrpRotatableImage::SetTexture( video::ITexture* texture )
{
	bodyMesh_.getMaterial().setTexture(0, texture);
}

bool CNrpRotatableImage::HasTexture()
{
	return bodyMesh_.getMaterial().getTexture(0) != NULL;
}
}//namespace gui

}//namespace irr
