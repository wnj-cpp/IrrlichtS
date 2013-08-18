#include <irrlicht.h>

/** PRG���
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

    ISceneNode* _rpgNode;   //RPG�ڵ����
//    core::vector3df rpgPositon; //PRG��ɫλ��

    /** ������ƶ��ͣ��Ƕ�+λ�ã����ƶ�����ͬʱ����
        ������ƶ��켣��ֱ�ߣ��Ƕ�+λ�õ��ƶ�������
    */
    f32 _camera_radius;    //��ǰӦ�õľ���
    f32 _camera_x, _camera_z;   //�����Ӧ�õ�x��z����
    f32 _camera_radian;   //�����Ӧ�õĽǶ�

    f32 _min_radian, _max_radian;   //��С��󻡶�
    f32 _min_radius, _max_radius;   //��С������

	bool _is_drag;
	bool is_first_;



};

} // end namespace scene
} // end namespace irr

