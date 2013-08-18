// Copyright (C) 2002-2010 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorCameraRPG.h"
#include "ICursorControl.h"
#include "ICameraSceneNode.h"
#include "SViewFrustum.h"
#include "ISceneManager.h"

#include <iostream>

namespace irr
{
namespace scene
{


//! constructor
CSceneNodeAnimatorCameraRPG::CSceneNodeAnimatorCameraRPG(gui::ICursorControl* cursor, ISceneNode* rnode, f32 rotate, f32 zoom, f32 translate)
	: CursorControl(cursor), OldCamera(0), MousePos(0.5f, 0.5f),
	ZoomSpeed(zoom), RotateSpeed(rotate), TranslateSpeed(translate),
	CurrentZoom(700.0f), RotX(0.0f), RotY(0.0f),
	Zooming(false), Rotating(false), Moving(false), Translating(false), rpgNode(rnode)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorCameraRPG");
	#endif

	if (CursorControl)
	{
		CursorControl->grab();
		MousePos = CursorControl->getRelativePosition();
	}

    this ->min_radius = 100.0f;
    this ->max_radius = 1000.0f;
    this ->camera_radius = 500.0f;

	allKeysUp();
}

//! destructor
CSceneNodeAnimatorCameraRPG::~CSceneNodeAnimatorCameraRPG()
{
	if (CursorControl)
		CursorControl->drop();
}

bool CSceneNodeAnimatorCameraRPG::OnEvent(const SEvent& event)
{
	if (event.EventType != EET_MOUSE_INPUT_EVENT)
		return false;


	switch(event.MouseInput.Event)
	{
	case EMIE_LMOUSE_PRESSED_DOWN:
		MouseKeys[0] = true;
		break;
	case EMIE_RMOUSE_PRESSED_DOWN:
		MouseKeys[2] = true;
		break;
	case EMIE_MMOUSE_PRESSED_DOWN:
		MouseKeys[1] = true;
		break;
	case EMIE_LMOUSE_LEFT_UP:
		MouseKeys[0] = false;
		break;
	case EMIE_RMOUSE_LEFT_UP:
		MouseKeys[2] = false;
		break;
	case EMIE_MMOUSE_LEFT_UP:
		MouseKeys[1] = false;
		break;
    case EMIE_MOUSE_WHEEL:
        camera_radius += 100 * event.MouseInput.Wheel;// 100代表wheel滚动产生的距离影响强度
        if (camera_radius < min_radius)
        {
            camera_radius = min_radius;
        }
        if (camera_radius > max_radius)
        {
            camera_radius = max_radius;
        }
        Zooming = true;
        break;
	case EMIE_MOUSE_MOVED:
		MousePos = CursorControl->getRelativePosition();
		break;
	case EMIE_LMOUSE_DOUBLE_CLICK:
	case EMIE_RMOUSE_DOUBLE_CLICK:
	case EMIE_MMOUSE_DOUBLE_CLICK:
	case EMIE_LMOUSE_TRIPLE_CLICK:
	case EMIE_RMOUSE_TRIPLE_CLICK:
	case EMIE_MMOUSE_TRIPLE_CLICK:
	case EMIE_COUNT:
		return false;
	}
	return true;
}

//! OnAnimate() is called just before rendering the whole scene.
void CSceneNodeAnimatorCameraRPG::animateNode(ISceneNode *node, u32 timeMs)
{
	//Alt + LM = Rotate around camera pivot
	//Alt + LM + MM = Dolly forth/back in view direction (speed % distance camera pivot - max distance to pivot)
	//Alt + MM = Move on camera plane (Screen center is about the mouse pointer, depending on move speed)

	if (!node || node->getType() != ESNT_CAMERA)
		return;

	ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(node);

	// If the camera isn't the active camera, and receiving input, then don't process it.
	if (!camera->isInputReceiverEnabled())
		return;

	scene::ISceneManager * smgr = camera->getSceneManager();
	if (smgr && smgr->getActiveCamera() != camera)
		return;


    camera ->setTarget(rpgNode ->getPosition());

	core::vector3df camera_position = camera->getPosition();
	core::vector3df target_position = camera->getTarget();


    if(Zooming)
    {
        std::cout << camera_position.getDistanceFrom(target_position) << "---" << camera_radius << std::endl;
        core::vector3df ms = target_position.getInterpolated(camera_position, (camera_position.getDistanceFrom(target_position) - camera_radius) / camera_position.getDistanceFrom(target_position));
        camera ->setPosition(ms);
        Zooming = false;

    }

	f32 nRotX = RotX;
	f32 nRotY = RotY;
    f32 nZoom = camera_radius;
	// Translation ---------------------------------



	if (isMouseKeyDown(0) && !Zooming)
	{
		if (!Rotating)//1
		{
			RotateStart = MousePos;
			Rotating = true;
		}
		else//2345678
		{
			nRotX += (RotateStart.X - MousePos.X) * RotateSpeed;
			nRotY += (RotateStart.Y - MousePos.Y) * RotateSpeed;
		}
	}
	else if (Rotating)//9
	{
        RotX = nRotX +=(RotateStart.X - MousePos.X) * RotateSpeed;
        RotY = nRotY +=(RotateStart.Y - MousePos.Y) * RotateSpeed;
		Rotating = false;
	}

	// Set camera_position ------------------------------------

	camera_position = target_position;
	camera_position.X += nZoom;

	camera_position.rotateXYBy(nRotY, target_position);
	camera_position.rotateXZBy(-nRotX, target_position);

	camera->setPosition(camera_position);
	camera->setTarget(target_position);

	// Rotation Error ----------------------------

	// jox: fixed bug: jitter when rotating to the top and bottom of y
	camera_position.set(0,1,0);
	camera_position.rotateXYBy(-nRotY);
	camera_position.rotateXZBy(-nRotX+180.f);
	camera->setUpVector(camera_position);
}

















bool CSceneNodeAnimatorCameraRPG::isMouseKeyDown(s32 key) const
{
	return MouseKeys[key];
}


void CSceneNodeAnimatorCameraRPG::allKeysUp()
{
	for (s32 i=0; i<3; ++i)
		MouseKeys[i] = false;
}


//! Sets the rotation speed
void CSceneNodeAnimatorCameraRPG::setRotateSpeed(f32 speed)
{
	RotateSpeed = speed;
}


//! Sets the movement speed
void CSceneNodeAnimatorCameraRPG::setMoveSpeed(f32 speed)
{
	TranslateSpeed = speed;
}


//! Sets the zoom speed
void CSceneNodeAnimatorCameraRPG::setZoomSpeed(f32 speed)
{
	ZoomSpeed = speed;
}


//! Gets the rotation speed
f32 CSceneNodeAnimatorCameraRPG::getRotateSpeed() const
{
	return RotateSpeed;
}


// Gets the movement speed
f32 CSceneNodeAnimatorCameraRPG::getMoveSpeed() const
{
	return TranslateSpeed;
}


//! Gets the zoom speed
f32 CSceneNodeAnimatorCameraRPG::getZoomSpeed() const
{
	return ZoomSpeed;
}


ISceneNodeAnimator* CSceneNodeAnimatorCameraRPG::createClone(ISceneNode* node, ISceneManager* newManager)
{
	CSceneNodeAnimatorCameraRPG * newAnimator =
		new CSceneNodeAnimatorCameraRPG(CursorControl, rpgNode, RotateSpeed, ZoomSpeed, TranslateSpeed);
	return newAnimator;
}


} // end namespace
} // end namespace
