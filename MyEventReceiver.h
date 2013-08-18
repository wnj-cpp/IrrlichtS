#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;

class MyEventReceiver : public IEventReceiver
{

public:

	MyEventReceiver(IrrlichtDevice* pDevice)
        : device(pDevice)
    {
        driver = pDevice->getVideoDriver();
        smgr = pDevice->getSceneManager();

        if(smgr)
        {
            camera = smgr->getActiveCamera();
        }
	}

	bool OnEvent(const SEvent& event)
	{
        line3d<f32> line;
//        line.start = camera->getPosition();
//        line.end = line.start + (camera->getTarget() - line.start).normalize() * 1000.0f;
        vector3df intersection;
        triangle3df tri;
        const ISceneNode* oNode = smgr->getSceneNodeFromType(ESNT_TERRAIN, 0);

        ISceneNode* roleNode = smgr->getSceneNodeFromType(ESNT_ANIMATED_MESH, 0);

        scene::ITriangleSelector* selector = oNode->getTriangleSelector();

		if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
        {

            printf("---2");
			switch (event.KeyInput.Key)
			{

                case irr::KEY_KEY_W:
                    printf("---3");

                    return true;
                default:
                    break;
			}
		}
		else if(event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
            s32 _x = event.MouseInput.X;
            s32 _y = event.MouseInput.Y;
		    if (event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP)
            {
    //		    ISceneCollisionManager* cmgr = smgr->getSceneCollisionManager();

                line = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(core::vector2di(_x, _y), camera);

                if (smgr->getSceneCollisionManager()->getCollisionPoint(line, selector, intersection, tri, oNode))
                {
//                    std::cout << ("[(");
//                    std::cout << (intersection.X);
//                    std::cout << (")(");
//                    std::cout << (intersection.Y);
//                    std::cout << (")(");
//                    std::cout << (intersection.Z);
//                    std::cout << (")]");
    //                    bill->setPosition(intersection);

                    if(roleNode){
    //                    roleNode->setPosition(intersection);
                        ISceneNodeAnimator* roleTrace = smgr->createFlyStraightAnimator(roleNode->getPosition(), intersection, 3000);
                        roleNode->addAnimator(roleTrace);
                        roleTrace->drop();

                    }
    //                    driver->setTransform(video::ETS_WORLD, core::matrix4());
    //                    driver->draw3DTriangle(tri, video::SColor(0,255,0,0));
                }
            }
            else if(event.MouseInput.isLeftPressed() && event.MouseInput.Event == EMIE_MOUSE_MOVED)
            {
                std::cout << "[";
                std::cout << _x;
                std::cout << "]";
            }

		}
		return false;
	}

private:

    IrrlichtDevice* device;

    IVideoDriver* driver;

    irr::scene::ISceneManager* smgr;

    ICameraSceneNode* camera;
};
