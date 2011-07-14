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

/*
* Original code copied from QTNeedleIndicator4
* THIS FILE WAS CREATED BY Dalerank 22 may 2010.
*/
//#include "stdafx.h"
#include "nrpNeedleIndicator.h"
#include "NrpRotatableImage.h"

#include <assert.h>

namespace irr
{

namespace gui
{


CNrpNeedleIndicator::CNrpNeedleIndicator(const wchar_t* text, IGUIEnvironment* environment,
										 IGUIElement* parent, s32 id, const core::rect<s32>& rectangle,
										 bool background) : IGUIElement( EGUIET_ELEMENT, environment, parent, id, rectangle )
{
    majorTicks_ = 11.0;
    minorTicks_ = 8.0;
    gapAngle_   = 90;
	startGapAngle_ = 90;
    startAngle_ = 90 + gapAngle_/2;
    stopAngle_  = 360 - gapAngle_;
    rotDeg_ = stopAngle_ / (majorTicks_-1);
	rotRad_ = (rotDeg_/360.0f)*2* core::PI;
    max_     = 100;
    min_     = 0;
    value_   = 0;
    step_    = max_ / (majorTicks_-1);
    labelOffset_ = 115 * 0.6;
    digitFont_ = environment->getBuiltInFont();
    labelFont_ = environment->getBuiltInFont();
    label_ = "";
    scaleFormat_ = "%.2f";
    animated_ = true;
	needleImg_ = new CNrpRotatableImage( core::recti( 0, 0, rectangle.getWidth(), rectangle.getHeight() ),
										 environment, -1, this );

	background_ = NULL;
	label_ = text;
	animFrame_ = 0;
	currValue_ = 0;
	frameRate_ = 50;
	numPie_ = 20;
	
	needleColor_=video::SColor(0xFF,0xFF,0,0);

	drawLastTick=true; /* addition by Brainsaw */

	UpdateMesh_();
}

void CNrpNeedleIndicator::Animate_()
{
	if( animated_ && currValue_ != value_ )
	{
		animFrame_++;
		if( animFrame_ < frameRate_ )
			currValue_ += ( value_ - currValue_ )/(float)frameRate_;  // lock the needle after animation on final position
		else
			currValue_ = value_;
	}
}

f32 CNrpNeedleIndicator::GetValue(void)
{
    return value_;
}

void CNrpNeedleIndicator::SetMajorTicks(int t)
{
    majorTicks_ = (f32)t;
    step_ = max_/(majorTicks_-1);
    rotDeg_ = stopAngle_ / (majorTicks_-1);
	rotRad_ = (rotDeg_/360.f)*2* core::PI;
}

void CNrpNeedleIndicator::SetMinorTicks(int t)
{
    minorTicks_ = static_cast<f32>(t);
}

void CNrpNeedleIndicator::SetDigitFont(IGUIFont* f)
{
    digitFont_ = f;
}

void CNrpNeedleIndicator::SetLabelFont(IGUIFont* f)
{
    labelFont_ = f;
}

void CNrpNeedleIndicator::SetAnimated(bool amount)
{
    animated_ = amount;
}

bool CNrpNeedleIndicator::IsAnimated(void)
{
    return animated_;
}

void CNrpNeedleIndicator::SetValue(f32 v)
{
    if(v > max_) v = max_;    // coerce
    if(v < min_) v = min_;
    if( animated_ )
	{
		//currValue_ = value_;
        value_ = v;
        animFrame_ = 0;
    }
	else
	{                // instant update
        currValue_ = value_ = v;
    }
}

void CNrpNeedleIndicator::SetLabel(core::stringw l)
{
    label_ = l;
}

void CNrpNeedleIndicator::SetDigitFormat(core::stringc format)
{
    scaleFormat_ = format;
}

void CNrpNeedleIndicator::SetRange(f32 mi, f32 ma)
{
    min_  = mi;
    max_  = ma;
    step_ = (max_-min_)/(majorTicks_-1);
}

void CNrpNeedleIndicator::SetMinValue(f32 mi)
{
    SetRange(mi, max_);
}

void CNrpNeedleIndicator::SetMaxValue(f32 ma)
{
    SetRange(min_, ma);
}

void CNrpNeedleIndicator::SetGapAngle(f32 gap)
{
    gapAngle_   = gap;
    startAngle_ = 90 + gapAngle_/2;
    stopAngle_  = 360 - gapAngle_;
    rotDeg_ = stopAngle_/(majorTicks_-1);
	rotRad_ = (rotDeg_/360.f)*2*core::PI;
}

void CNrpNeedleIndicator::SetLabelOffset(f32 offset)
{
    assert(offset >= 0 && offset <= 1);
    if( offset < 0 ) offset = 0;
    if( offset > 1 ) offset = 1;
    labelOffset_ = 115*offset;
}

f32 CNrpNeedleIndicator::Value2Angle_(f32 val)
{
    if( val > max_ ) val = max_;
    return startAngle_ + ((max_-val)/max_)*stopAngle_;
}

void CNrpNeedleIndicator::DrawNeedle_()
{
	f32 angle = Value2Angle_( currValue_ ) - startGapAngle_;

	if( needleImg_->HasTexture() )
	{
		if( (angle - 180) != needleImg_->getRotate() )
			needleImg_->SetRotate( (angle - 180.f) );

		needleImg_->draw();
	}
	else
	{
		core::position2di center = AbsoluteRect.getCenter();
	    int side=AbsoluteRect.getWidth()<AbsoluteRect.getHeight()?AbsoluteRect.getWidth()/2:AbsoluteRect.getHeight()/2;

		core::position2di leftCenter;
		leftCenter.X = side * 0.05 * sin( (angle - 90) * core::DEGTORAD );
		leftCenter.Y = side * 0.05 * cos( (angle - 90) * core::DEGTORAD );

		core::position2di rigthCenter;
		rigthCenter.X = side * 0.05 * sin( (angle + 90) * core::DEGTORAD );
		rigthCenter.Y = side * 0.05 * cos( (angle + 90) * core::DEGTORAD );

		core::position2di neddlePoint;
		neddlePoint.X = side * 0.9 * sin( angle * core::DEGTORAD );
		neddlePoint.Y = side * 0.9 * cos( angle * core::DEGTORAD );

		video::IVideoDriver* driver = Environment->getVideoDriver();
		driver->draw2DLine( leftCenter + center, rigthCenter + center, needleColor_ );
		driver->draw2DLine( leftCenter + center, neddlePoint + center, needleColor_ );
		driver->draw2DLine( neddlePoint + center, rigthCenter + center, needleColor_ );
	}
}

void CNrpNeedleIndicator::DrawLabel_( f32 valuel, core::position2di beginp, core::position2di endp )
{
	core::position2di center = AbsoluteRect.getCenter();
	wchar_t strValue[32];

	swprintf( strValue, 31, core::stringw( scaleFormat_ ).c_str(), valuel  ); /* convert digit to string     */
	f32 h = digitFont_->getDimension(L"A").Height;

	core::recti labelRect;

	if( beginp.X == endp.X )
	{
		labelRect.UpperLeftCorner = beginp + center + core::position2di( -10, 0);
		labelRect.LowerRightCorner = beginp + center + core::position2di( 10, 0);
	}
	else
	{
		labelRect.UpperLeftCorner = beginp + center + (( beginp.X >= endp.X ) ? core::position2di( 0, 0) : core::position2di( -20, 0));
		labelRect.LowerRightCorner = beginp + center + (( beginp.X >= endp.X ) ? core::position2di( 20, h ) : core::position2di( 0, 0));
	}


	digitFont_->draw( strValue, labelRect, 0xff000000, false, false, 0 );

}

void CNrpNeedleIndicator::UpdateMesh_()
{
	const f32 halfWidth = 1 / 1.f;
	core::position2di center( halfWidth / 2, halfWidth / 2 );
	mesh_.Vertices.set_used( numPie_+1 ); //center+radial
	mesh_.Indices.set_used( numPie_ * 3 ); //triangle points count
	const video::SColor white( 0xFF, 0xFF, 0xFF, 0xFF);

	//add center point
	mesh_.Vertices[0] = video::S3DVertex( 0, 0, 0.f, 0.f, 0.f, 1.f, white, 0.f, 1.f);

	//add triangle points
	for( int k=0; k < numPie_; k++ )
	{
		mesh_.Vertices[k+1] = video::S3DVertex( halfWidth * sin( core::PI * 2 / (numPie_-1) * k ),
											  halfWidth * cos( core::PI * 2 / (numPie_-1) * k  ),
											  0.f, 0.f, 0.f, 1.f, 0xff000000, 1.f, 1.f);
		mesh_.Indices[3*k] = 0;
		mesh_.Indices[3*k+1] = k;
		mesh_.Indices[3*k+2] = k+1;
	}

	mesh_.getMaterial().Lighting = false;
	mesh_.getMaterial().MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;									//снимаем счетчик с рефера
	//mesh_.getMaterial().setTexture(0, Environment->getVideoDriver()->getTexture( "media/bump.png"));

	matrix_.makeIdentity();
	matrix_.setRotationDegrees(core::vector3df(0.0f, 0.0f, 0.f));
}

void CNrpNeedleIndicator::DrawMesh_()
{
	video::IVideoDriver* driver = Environment->getVideoDriver();

	const core::rect<s32> oldViewPort = driver->getViewPort();				//сохраняем параметры экрана

	driver->setViewPort(AbsoluteRect);										//устанавливаем параметры отображения как нашу область элемента

	const core::matrix4 oldProjMat = driver->getTransform(video::ETS_PROJECTION); //сохраняем текущую проекцию экрана
	driver->setTransform(video::ETS_PROJECTION, core::matrix4());			//ставим пустую проекцию

	const core::matrix4 oldViewMat = driver->getTransform(video::ETS_VIEW);	//сохраняем матрицу отображения
	driver->setTransform(video::ETS_VIEW, core::matrix4());					//ставим пустую матрицу отображения
	driver->setTransform(video::ETS_WORLD, matrix_);						//савим матрицу оторбажения с поворотом на нужный угол

	driver->setMaterial( mesh_.Material );								//устанавливаем материал отрисовки
	driver->drawMeshBuffer( &mesh_ );										//рисуем карту

	driver->setTransform(video::ETS_VIEW, oldViewMat);						//возвращаем матрицу вида
	driver->setTransform(video::ETS_PROJECTION, oldProjMat);				//восстанавливаем проекцию
	driver->setViewPort(oldViewPort);										//востанавливаем параметры отображения экрана
}

void CNrpNeedleIndicator::DrawBackground_(void)
{
    int minside = AbsoluteRect.getWidth();
	int maxside = AbsoluteRect.getHeight();
    /* Keep side size an even number by trunkating odd pixel */
    minside &= ~0x01;
	maxside &= ~0x01;

	video::IVideoDriver* driver = Environment->getVideoDriver();

	if( background_ )
		driver->draw2DImage( background_, AbsoluteRect,
							 backgroundRect_,
							 NULL, NULL, true );
	else
		DrawMesh_();

    //int line = 10; /* FIX #1 */

    /* Draw scale majorTicks using coordinats rotation */
 	core::position2di center = AbsoluteRect.getCenter();

	f32 tmpValue = min_;
	f32 bAngle = startAngle_ - startGapAngle_;
	int tickCount = minorTicks_*(majorTicks_-1)+majorTicks_;
    f32 t_rot = stopAngle_/(tickCount-1);
	f32 r_value = (max_ - min_)/(tickCount-1);
    if (!drawLastTick) tickCount--;
    for(int i = 0; i < tickCount; i++)
	{
		core::position2di elPoint( minside * sin( bAngle * core::DEGTORAD ), maxside * cos( bAngle * core::DEGTORAD ) );
		elPoint += center;
		s32 tmpRad = center.getDistanceFrom( elPoint );
		s32 radiusMin = (s32)(tmpRad / 2 * 0.9f);
		s32 radiusMax = (s32)(tmpRad / 2 * 0.8f);
		s32 radiusTickMax = (s32)(tmpRad / 2 * 0.7f);

		core::position2di bp, ep;

        if( minorTicks_ )
		{
            if( i%(int)(minorTicks_+1) == 0 )
			{
				bp = core::position2di( (s32)(radiusTickMax * sin( bAngle * core::DEGTORAD )),
										(s32)(radiusTickMax * cos( bAngle * core::DEGTORAD )) );
				ep = core::position2di( (s32)(radiusMin * sin( bAngle * core::DEGTORAD)),
										(s32)(radiusMin * cos( bAngle * core::DEGTORAD)) );

				DrawLabel_( max_ - tmpValue, bp, ep );
			}
            else
			{
				bp = core::position2di( (s32)(radiusMin * sin( bAngle * core::DEGTORAD )),
										(s32)(radiusMin * cos( bAngle * core::DEGTORAD )) );
				ep = core::position2di( (s32)(radiusMax * sin( bAngle * core::DEGTORAD)),
										(s32)(radiusMax * cos( bAngle * core::DEGTORAD)) );
			}
        }
		else
		{
			bp = core::position2di( (s32)(radiusTickMax * sin( bAngle * core::DEGTORAD )),
									(s32)(radiusTickMax * cos( bAngle * core::DEGTORAD )) );
			ep = core::position2di( (s32)(radiusMin * sin( bAngle * core::DEGTORAD)),
									(s32)(radiusMin * cos( bAngle * core::DEGTORAD)) );
       }

		driver->draw2DLine( bp + center, ep + center, 0xff00ff00 );
        bAngle += t_rot;
		tmpValue += r_value;
    }

    /* Draw meter label */
    if( label_.size() )
	{
		core::recti labelRect = AbsoluteRect;
		labelRect.UpperLeftCorner.Y = AbsoluteRect.getCenter().Y;
		labelFont_->draw( label_.c_str(), labelRect, 0xffff0000, true, true, NULL );
	}
}

void CNrpNeedleIndicator::SetNeedleTexture( video::ITexture* ptx )
{
	needleImg_->SetTexture( ptx );
}

void CNrpNeedleIndicator::draw()
{
	//IGUIElement::draw();
	if( IsVisible )
	{
		Animate_();
		DrawBackground_();
		DrawNeedle_();
	}
}

void CNrpNeedleIndicator::SetBackgroundTexture( video::ITexture* ptx )
{
	if ( background_)
		background_->drop();

	background_ = ptx;
	if( ptx )
		backgroundRect_ = core::rect<s32>(core::position2d<s32>(0,0), ptx->getOriginalSize());

	if (background_)
		background_->grab();
}

void CNrpNeedleIndicator::SetStartGapAngle( f32 angle )
{
	startGapAngle_ = angle; if( startGapAngle_ > 360.f) startGapAngle_-= 360.f;
}

}//end namespace gui

}//end namespace irr
