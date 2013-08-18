#include "CameraAnimator.h"

#include <iostream>
#include <cmath>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace irr
{
namespace scene
{

//! Constructor
CameraAnimator::CameraAnimator(ISceneNode* rpgNode, IrrlichtDevice* device, ISceneManager* smgr, u32 time) :
    _rpgNode(rpgNode),
    device_(device),//
    scene_manager_(smgr),//
    start_time_(time),
    camera_speed_(0),
    target_speed_(0),
    target_accel_(0),
    current_node_(NULL),
    last_node_(NULL),
    x_target(0),
    z_target(-700),
    is_drag_(false),
    is_first_(false)
{
    this ->_min_radius = 100.0f;
    this ->_max_radius = 1000.0f;
    this ->_min_radian = 0.0f;
    this ->_max_radian = 90.0f;
    this ->_camera_radius = 111.0f;
    this ->_camera_radian = 111.0f;
    if(_rpgNode == 0)
    {

    }
};

//! Destructor
CameraAnimator::~CameraAnimator()
{
};

void CameraAnimator::animateNode(ISceneNode* node, u32 timeMs)
{
	if ( 0 == node || node->getType() != ESNT_CAMERA)
	{
		return;
	}

	irr::scene::ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(node);

	core::vector3df camera_position = camera->getPosition();
	core::vector3df target_position = camera->getTarget();

//	camera->setPosition(_rpgNode);
	camera->setTarget(this ->_rpgNode ->getPosition());

    //先尝试永恒匀速直线运动
    //可以尝试慢慢加速减速--
    //可以尝试根据距离决定速度--
    //根据目标位置target_position和应该的距离_camera_radius计算摄像机位置
    core::vector3df ms = target_position.getInterpolated(camera_position, (camera_position.getDistanceFrom(target_position) - _camera_radius) / camera_position.getDistanceFrom(target_position));
    camera ->setPosition(ms);



//	f32 dt = (timeMs - start_time_) / 1000.0;
//
//	f32 sign;
//	f32 x_current = target_position.X;
//	f32 x_speed = 0;
//
//	if (is_drag_ && current_node_ == NULL) // drag
//	{
//		core::position2di mouse_position = device_->getCursorControl()->getPosition();
//		s32 xdiff = drag_start_point_.X - mouse_position.X;
//		drag_start_point_ = mouse_position;
//
//		x_target += 0.01*xdiff / core::max_(dt, 0.0000001f);
//	}
//
//	if (current_node_!=NULL) // click
//	{
//		is_first_ = false;
//		x_target = current_node_->getPosition().X;
//	}
//	sign = (x_target < x_current) ? -1 : 1;
//	x_speed = sign*sqrt(abs(x_target - x_current)) * 50;
//	target_position.X += x_speed * dt;
//
//	// 1.2 camera
//	sign = (target_position.X < camera_position.X) ? -1 : 1;
//	camera_speed_ = sign*sqrt(abs(target_position.X - camera_position.X)) * 80;
//	camera_position.X += camera_speed_ * dt;
//
//	if (x_target < 0)
//	{
//		x_target = 0;
//	}
//	if (x_target > ::ceil((double)wall_vect_->size() / 3 - 1) * 400)
//	{
//		x_target = ::ceil((double)wall_vect_->size() / 3 - 1) * 400;
//	}
//
//	// 2. Y motion
//	f32 y_target = 0;
//	f32 y_current = camera_position.Y;
//	f32 y_speed = 0;
//	if (current_node_ != NULL)
//	{
//		y_target = current_node_->getPosition().Y;
//	}
//
//	sign = (y_target < y_current) ? -1 : 1;
//	y_speed = sign*sqrt(abs(y_target - y_current)) * 80;
//	y_current += y_speed * dt;
//
//	camera_position.Y = y_current;
//	target_position.Y = y_current;
//
//	// 3. Z motion
//	if (is_drag_)
//	{
//		z_target = -700;
//	}
//	f32 z_current = camera_position.Z;
//	f32 z_speed = 0;
//	if (current_node_ != NULL)
//	{
//		z_target = -400;
//	}
//
//	sign = (z_target < z_current) ? -1 : 1;
//	z_speed = sign*sqrt(abs(z_target - z_current)) * 50;
//	z_current += z_speed * dt;
//
//	camera_position.Z = z_current;
//
//	// 5. set position
//	camera->setPosition(camera_position);
//	camera->setTarget(target_position);
//	//std::cout << "camera: " << camera_position.X << "\t\t" << "target: " << target_position.X << std::endl;
//
//	// 4. Node motion
//	if (last_node_ != NULL)
//	{
//		f32 z_current = last_node_->getAbsolutePosition().Z;
//		f32 z_target = 0;
//		f32 z_speed = 0;
//
//		f32 sign = (z_target < z_current) ? -1 : 1;
//		z_speed = sign*sqrt(abs(z_target - z_current)) * 80;
//		z_current += z_speed * dt;
//
//		core::vector3df pos = last_node_->getAbsolutePosition();
//		pos.Z = z_current;
//		last_node_->setPosition(pos);
//	}
//
//	if (current_node_ != NULL)
//	{
//		f32 z_current = current_node_->getAbsolutePosition().Z;
//		f32 z_target = -180;
//		f32 z_speed = 0;
//
//		f32 sign = (z_target < z_current) ? -1 : 1;
//		z_speed = sign*sqrt(abs(z_target - z_current)) * 80;
//		z_current += z_speed * dt;
//
//		core::vector3df pos = current_node_->getAbsolutePosition();
//		pos.Z = z_current;
//		current_node_->setPosition(pos);
//	}
//
//	start_time_ = timeMs;
}

bool CameraAnimator::OnEvent(const SEvent& event)
{
	switch(event.EventType)
	{
	case EET_MOUSE_INPUT_EVENT:// mouse event
		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
		    _is_drag = true;
			core::position2d<s32> mouse_position(event.MouseInput.X, event.MouseInput.Y);
			ISceneNode* node = scene_manager_->getSceneCollisionManager()->getSceneNodeFromScreenCoordinatesBB(mouse_position, -1);
			last_node_ = current_node_;

			if (node && node->getType() == ESNT_CUBE )
			{
				current_node_ = node;
			}
			else
			{
				current_node_ = NULL;
			}


			drag_start_point_ = mouse_position;
		}
		else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			_is_drag = false;
		}
		else
            if (event.MouseInput.Event == EMIE_MOUSE_WHEEL)// mouse wheel
		{
			if (current_node_ == NULL)
			{
				_camera_radius += 100 * event.MouseInput.Wheel;// 100代表wheel滚动产生的距离影响强度
				if (_camera_radius < _min_radius)
				{
					_camera_radius = _min_radius;
				}
				if (_camera_radius > _max_radius)
				{
					_camera_radius = _max_radius;
				}
//core::vector3df aa = vector3df(1,2,4);
//core::vector3df bb = aa.getInterpolated(vector3df(2,4,7), 0.2f);
//std::cout << "[";
//std::cout << bb.X;
//std::cout << ",";
//std::cout << bb.Y;
//std::cout << ",";
//std::cout << bb.Z;
//std::cout << "]";

//                std::cout << "[";
//                std::cout << event.MouseInput.Wheel;
//                std::cout << "-";
//                std::cout << _camera_radius;
//                std::cout << "]";
			}
		}
		break;
		return true;
	}

	return false;
}

bool CameraAnimator::isEventReceiverEnabled() const
{
    return true;
}

ISceneNodeAnimator* CameraAnimator::createClone(ISceneNode* node, ISceneManager* newManager/* =0 */)
{
	return NULL;
}

}
}
