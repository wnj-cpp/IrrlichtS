// Copyright (C) 2002-2010 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorCameraRPG.h"
#include "ICursorControl.h"
#include "ICameraSceneNode.h"
#include "SViewFrustum.h"
#include "ISceneManager.h"

namespace irr
{
namespace scene
{


//! constructor
CSceneNodeAnimatorCameraPRG::CSceneNodeAnimatorCameraPRG(gui::ICursorControl* cursor, f32 rotate, f32 zoom, f32 translate = 1500.0f)
	: CursorControl(cursor), OldCamera(0), MousePos(0.5f, 0.5f),
	ZoomSpeed(zoom), RotateSpeed(rotate), TranslateSpeed(translate),
	CurrentZoom(70.0f), RotX(0.0f), RotY(0.0f),
	Zooming(false), Rotating(false), Moving(false), Translating(false)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorCameraRPG");
	#endif

	if (CursorControl)
	{
		CursorControl->grab();
		MousePos = CursorControl->getRelativePosition();
	}

	allKeysUp();
}

//! destructor
CSceneNodeAnimatorCameraPRG::~CSceneNodeAnimatorCameraPRG()
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
	case EMIE_MOUSE_MOVED:
		MousePos = CursorControl->getRelativePosition();
		break;
	case EMIE_MOUSE_WHEEL:
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

	if (OldCamera != camera)
	{
		LastCameraTarget = OldTarget = camera->getTarget();
		OldCamera = camera;
	}
	else
	{
		OldTarget += camera->getTarget() - LastCameraTarget;
	}

	f32 nRotX = RotX;
	f32 nRotY = RotY;
	f32 nZoom = CurrentZoom;

	if ( (isMouseKeyDown(0) && isMouseKeyDown(2)) || isMouseKeyDown(1) )//左键按下且右键按下 或者中键按下,,,,,这些都是距离远近的操作
	{
		if (!Zooming)//首次按下会被执行
		{
			ZoomStart = MousePos;//记录缩放前鼠标位置
			Zooming = true;//标记为缩放状态
		}
		else
		{
			const f32 targetMinDistance = 0.1f;//目标最小距离
			nZoom += (ZoomStart.X - MousePos.X) * ZoomSpeed;

			if (nZoom < targetMinDistance) // jox: fixed bug: bounce back when zooming to close
				nZoom = targetMinDistance;
		}
	}
	else if (Zooming)
	{
		const f32 old = CurrentZoom;
		CurrentZoom = CurrentZoom + (ZoomStart.X - MousePos.X ) * ZoomSpeed;
		nZoom = CurrentZoom;

		if (nZoom < 0)
			nZoom = CurrentZoom = old;
		Zooming = false;
	}

	// Translation ---------------------------------

	core::vector3df translate(OldTarget);
	const core::vector3df upVector(camera->getUpVector());
	const core::vector3df target = camera->getTarget();

	core::vector3df pos = camera->getPosition();
	core::vector3df tvectX = pos - target;
	tvectX = tvectX.crossProduct(upVector);
	tvectX.normalize();

	const SViewFrustum* const va = camera->getViewFrustum();
	core::vector3df tvectY = (va->getFarLeftDown() - va->getFarRightDown());
	tvectY = tvectY.crossProduct(upVector.Y > 0 ? pos - target : target - pos);
	tvectY.normalize();

	if (isMouseKeyDown(2) && !Zooming)
	{
		if (!Translating)
		{
			TranslateStart = MousePos;
			Translating = true;
		}
		else
		{
			translate +=  tvectX * (TranslateStart.X - MousePos.X)*TranslateSpeed +
			              tvectY * (TranslateStart.Y - MousePos.Y)*TranslateSpeed;
		}
	}
	else if (Translating)
	{
		translate += tvectX * (TranslateStart.X - MousePos.X)*TranslateSpeed +
		             tvectY * (TranslateStart.Y - MousePos.Y)*TranslateSpeed;
		OldTarget = translate;
		Translating = false;
	}

	// Rotation ------------------------------------

	if (isMouseKeyDown(0) && !Zooming)
	{
		if (!Rotating)
		{
			RotateStart = MousePos;
			Rotating = true;
			nRotX = RotX;
			nRotY = RotY;
		}
		else
		{
			nRotX += (RotateStart.X - MousePos.X) * RotateSpeed;
			nRotY += (RotateStart.Y - MousePos.Y) * RotateSpeed;
		}
	}
	else if (Rotating)
	{
		RotX += (RotateStart.X - MousePos.X) * RotateSpeed;
		RotY += (RotateStart.Y - MousePos.Y) * RotateSpeed;
		nRotX = RotX;
		nRotY = RotY;
		Rotating = false;
	}

	// Set pos ------------------------------------

	pos = translate;
	pos.X += nZoom;

	pos.rotateXYBy(nRotY, translate);
	pos.rotateXZBy(-nRotX, translate);

	camera->setPosition(pos);
	camera->setTarget(translate);

	// Rotation Error ----------------------------

	// jox: fixed bug: jitter when rotating to the top and bottom of y
	pos.set(0,1,0);
	pos.rotateXYBy(-nRotY);
	pos.rotateXZBy(-nRotX+180.f);
	camera->setUpVector(pos);
	LastCameraTarget = camera->getTarget();
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
		new CSceneNodeAnimatorCameraRPG(CursorControl, RotateSpeed, ZoomSpeed, TranslateSpeed);
	return newAnimator;
}


} // end namespace
} // end namespace
