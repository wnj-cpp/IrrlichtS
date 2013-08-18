#include <irrlicht.h>
//#include "CameraAnimator.h"
#include "CSceneNodeAnimatorCameraRPG.h"
#include "MyEventReceiver.h"

#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//#ifdef _IRR_WINDOWS_
//#pragma comment(lib, "Irrlicht.lib")
//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
//#endif

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

int main()
{
	irr::IrrlichtDevice* device =
//		createDevice( video::EDT_DIRECT3D9, dimension2d<u32>(640, 480), 16,
        createDevice( video::EDT_DIRECT3D9, dimension2d<u32>(1366, 768), 32,
        //       createDevice( video::EDT_SOFTWARE, dimension2d<u32>(1366, 768), 32,
			true, false, false, 0);

	if (!device)
		return 1;

	IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

    //窗口标题
	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

    //天空盒
    driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
    ISceneNode* skybox = smgr->addSkyBoxSceneNode(
       driver->getTexture("res/sky6_up.jpg"),
       driver->getTexture("res/sky6_dn.jpg"),
       driver->getTexture("res/sky6_lf.jpg"),
       driver->getTexture("res/sky6_rt.jpg"),
       driver->getTexture("res/sky6_ft.jpg"),
       driver->getTexture("res/sky6_bk.jpg"));
    driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

    //使天空旋转
    ISceneNodeAnimator* skyTrace = smgr->createRotationAnimator(core::vector3df(0,0.005f,0));
    skybox->addAnimator(skyTrace);
    skyTrace->drop();

	//地形
	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
		"res/dixing.jpg",//100*70
		0,					// parent node
		-1,					// node id
		core::vector3df(-2500.f, -500.f, -1750.f),		// position
		core::vector3df(0.f, 0.f, 0.f),		// rotation
		core::vector3df(50.f, 5.f, 50.f),	// scale//5000*3500
		video::SColor ( 255, 255, 255, 255 ),	// vertexColor
		5,					// maxLOD
		scene::ETPS_17,				// patchSize
		7					// smoothFactor
		);

	terrain->setMaterialFlag(video::EMF_LIGHTING, false);

	terrain->setMaterialTexture(0,
			driver->getTexture("res/dixing_textrue.jpg"));
	terrain->setMaterialTexture(1,
			driver->getTexture("res/dixing_detail.jpg"));

	terrain->setMaterialType(video::EMT_DETAIL_MAP);

	terrain->scaleTexture(1.0f, 40.0f);
	//terrain->setDebugDataVisible ( true );

    //给地形绑定三角器，以方便碰撞检测
   ITriangleSelector* selector = smgr->createTerrainTriangleSelector(terrain, 0);
   terrain->setTriangleSelector(selector);
   selector->drop();

    //水
	IAnimatedMesh* mesh = smgr->addHillPlaneMesh( "water",
		core::dimension2d<f32>(60,60),
		core::dimension2d<u32>(160,160), 0, 0,
		core::dimension2d<f32>(0,0),
		core::dimension2d<f32>(10,10));

	ISceneNode* nd = smgr->addWaterSurfaceSceneNode(mesh->getMesh(0), 3.0f, 300.0f, 30.0f);
	nd->setPosition(core::vector3df(500,50,500));

	nd->setMaterialTexture(0, driver->getTexture("res/stones.jpg"));
	nd->setMaterialTexture(1, driver->getTexture("res/water.jpg"));

	nd->setMaterialType(video::EMT_REFLECTION_2_LAYER);

    //灯光
    ISceneNode* node = smgr->addLightSceneNode(0, core::vector3df(600,1000,600),
       video::SColorf(100.0f, 100.0f, 100.0f, 100.0f), 2000.0f);
    //静态文本
//	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!",
//		rect<s32>(10,10,260,22), true);

    //高达模型
	IAnimatedMesh* gandumMesh = smgr->getMesh("res/gandum.3ds");
	if (!gandumMesh)
	{
		device->drop();
		return 1;
	}
	scene::IAnimatedMeshSceneNode* roleNode = smgr->addAnimatedMeshSceneNode(gandumMesh);
	if (roleNode)
	{
		roleNode->setMaterialFlag(EMF_LIGHTING, false);
		roleNode->setMD2Animation(scene::EMAT_STAND);
		roleNode->setPosition(core::vector3df(500.f, 100.f, 500.f));
		roleNode->setMaterialTexture( 0, driver->getTexture("res/gandum.jpg"));
	}

//    core::array< scene::ISceneNode* > wall_item_list;
//    wall_item_list.push_back(roleNode);
//    wall_item_list.push_back(nd);

    //摄像机
//	scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0, vector3df(500,300,300), vector3df(500,100,500));
//    smgr->addCameraSceneNodeMaya();

	scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(0, vector3df(500,800,300));
    ISceneNodeAnimator* animator = new CSceneNodeAnimatorCameraRPG(device ->getCursorControl(), roleNode, -1500.0f, 200.0f, 1500.0f);
	camera->addAnimator(animator);
	animator->drop();

//	smgr->addCameraSceneNodeFPS();
//	device->getCursorControl()->setVisible(false);

    //接受用户事件
    MyEventReceiver receiver(device);
    device->setEventReceiver(&receiver);

    int lastFPS = -1;
	while(device->run())
	{
//        if (device->isWindowActive())
//        {
            driver->beginScene(true, true, SColor(255,100,101,140));

            smgr->drawAll();

            vector3d<f32> rolePosition = roleNode->getPosition();
            core::stringw str = L"(";
            str += rolePosition.X;
            str += L")(";
            str += rolePosition.Z;
            str += L")(";
            str += rolePosition.Y;
            str += L")";

//            camera->setTarget(roleNode->getPosition());
//            camera->setPosition(roleNode->getPosition() + core::vector3df(-200.f, 200.f, -200.f));
            device->setWindowCaption(str.c_str());

    //		for(int x = -50; x <= 50; x+=10){//z
    //            for(int y = -50; y <= 50; y+=10){
    //                driver->draw3DLine(vector3df(x,y,-50), vector3df(x,y,50), SColor(255,0,0,0));
    //            }
    //		}
    //		for(int x = -50; x <= 50; x+=10){//y
    //            for(int z = -50; z <= 50; z+=10){
    //                driver->draw3DLine(vector3df(x,-50,z), vector3df(x,50,z), SColor(255,255,0,0));
    //            }
    //		}
    //		for(int z = -50; z <= 50; z+=10){//x
    //            for(int y = -50; y <= 50; y+=10){
    //                driver->draw3DLine(vector3df(-50,y,z), vector3df(50,y,z), SColor(255,0,0,255));
    //            }
    //		}
    //		guienv->drawAll();

            driver->endScene();
//        }
	}

	device->drop();

	return 0;
}
