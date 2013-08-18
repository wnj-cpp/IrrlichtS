#include <irrlicht.h>

/** PRG相机
*/
namespace irr
{
namespace scene
{

//! Special scene node animator for RPG cameras
class CameraAnimator : public ISceneNodeAnimator
{
public:

	//! Constructor
	CameraAnimator(ISceneNode* rpgAnimator, IrrlichtDevice* device, ISceneManager* smgr, u32 time);

	//! Destructor
	~CameraAnimator();

	//! Animates the scene node, currently only works on cameras
	virtual void animateNode(ISceneNode* node, u32 timeMs);

	//! Event receiver
	virtual bool OnEvent(const SEvent& event);

	//! This animator will receive events when attached to the active camera
	virtual bool isEventReceiverEnabled() const;

	//! Creates a clone of this animator.
	/** Please note that you will have to drop
	(IReferenceCounted::drop()) the returned pointer after calling
	this. */
	virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager=0);

private:
	IrrlichtDevice* device_;
	ISceneManager* scene_manager_;
	core::array< ISceneNode* >* wall_vect_;
	u32 start_time_;

	ISceneNode* current_node_;
	ISceneNode* last_node_;
	f32 x_target;
	f32 z_target;
	f32 camera_speed_;
	f32 target_speed_;
	f32 target_accel_;


	core::position2di drag_start_point_;

    ISceneNode* _rpgNode;   //RPG节点对象
//    core::vector3df rpgPositon; //PRG角色位置

    /** 距离的移动和（角度+位置）的移动不会同时进行
        距离的移动轨迹是直线，角度+位置的移动是曲线
    */
    f32 _camera_radius;    //当前应该的距离
    f32 _camera_x, _camera_z;   //摄像机应该的x和z坐标
    f32 _camera_radian;   //摄像机应该的角度

    f32 _min_radian, _max_radian;   //最小最大弧度
    f32 _min_radius, _max_radius;   //最小最大距离

	bool _is_drag;
	bool is_first_;



};

} // end namespace scene
} // end namespace irr

