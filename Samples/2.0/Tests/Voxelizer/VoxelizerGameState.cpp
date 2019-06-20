
#include "VoxelizerGameState.h"
#include "CameraController.h"
#include "GraphicsSystem.h"

#include "OgreSceneManager.h"
#include "OgreItem.h"
#include "OgreMesh.h"
#include "OgreMeshManager.h"
#include "OgreMesh2.h"
#include "OgreMeshManager2.h"
#include "OgreMesh2Serializer.h"

#include "OgreCamera.h"
#include "OgreRenderWindow.h"

#include "OgreHlmsPbs.h"

#include "OgreRoot.h"
#include "Vct/OgreVctVoxelizer.h"
#include "Vct/OgreVctLighting.h"

#include "OgreWindow.h"

using namespace Demo;

#define TODO_do_this_in_voxelizer

namespace Demo
{
    Ogre::VctVoxelizer *voxelizer = 0;
    Ogre::VctLighting *vctLighting = 0;

    VoxelizerGameState::VoxelizerGameState( const Ogre::String &helpDescription ) :
        TutorialGameState( helpDescription )
    {
    }
    //-----------------------------------------------------------------------------------
    void VoxelizerGameState::createScene01(void)
    {
        Ogre::SceneManager *sceneManager = mGraphicsSystem->getSceneManager();

        Ogre::v1::MeshPtr v1Mesh;
        Ogre::MeshPtr Voxelizer;

        //---------------------------------------------------------------------------------------
        //Import Athene to v2 and render it without saving to disk.
        //---------------------------------------------------------------------------------------

        //Load the v1 mesh. Notice the v1 namespace
        //Also notice the HBU_STATIC flag; since the HBU_WRITE_ONLY
        //bit would prohibit us from reading the data for importing.
        v1Mesh = Ogre::v1::MeshManager::getSingleton().load(
                    "athene.mesh", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                    Ogre::v1::HardwareBuffer::HBU_STATIC, Ogre::v1::HardwareBuffer::HBU_STATIC );

        //Create a v2 mesh to import to, with a different name (arbitrary).
        Voxelizer = Ogre::MeshManager::getSingleton().createManual(
                    "athene.mesh Imported", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );

        bool halfPosition   = true;
        bool halfUVs        = true;
        bool useQtangents   = true;

        //Import the v1 mesh to v2
        Voxelizer->importV1( v1Mesh.get(), halfPosition, halfUVs, useQtangents );

        //We don't need the v1 mesh. Free CPU memory, get it out of the GPU.
        //Leave it loaded if you want to use athene with v1 Entity.
        v1Mesh->unload();

        Ogre::Item *cornellItem = sceneManager->createItem( "CornellBox.mesh",
                                                            Ogre::ResourceGroupManager::
                                                            AUTODETECT_RESOURCE_GROUP_NAME,
                                                            Ogre::SCENE_DYNAMIC );
        Ogre::SceneNode *sceneNode = sceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->
                                     createChildSceneNode( Ogre::SCENE_DYNAMIC );
        sceneNode->attachObject( cornellItem );

        //Create an Item with the model we just imported.
        //Notice we use the name of the imported model. We could also use the overload
        //with the mesh pointer:
        //  item = sceneManager->createItem( Voxelizer, Ogre::SCENE_DYNAMIC );
        Ogre::Item *item = sceneManager->createItem( "athene.mesh Imported",
                                                     Ogre::ResourceGroupManager::
                                                     AUTODETECT_RESOURCE_GROUP_NAME,
                                                     Ogre::SCENE_DYNAMIC );
        sceneNode = sceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->
                    createChildSceneNode( Ogre::SCENE_DYNAMIC );
        sceneNode->attachObject( item );
        sceneNode->setScale( Ogre::Vector3( 0.01f ) );
        sceneNode->setPosition( 0, item->getWorldAabbUpdated().mHalfSize.y, 0 );

        Ogre::Light *light = sceneManager->createLight();
        Ogre::SceneNode *lightNode = sceneManager->getRootSceneNode()->createChildSceneNode();
        lightNode->attachObject( light );
        light->setPowerScale( Ogre::Math::PI ); //Since we don't do HDR, counter the PBS' division by PI
        light->setType( Ogre::Light::LT_DIRECTIONAL );
        light->setDirection( Ogre::Vector3( -1, -1, -1 ).normalisedCopy() );

        mCameraController = new CameraController( mGraphicsSystem, false );

        TutorialGameState::createScene01();

        TODO_do_this_in_voxelizer;
        sceneManager->updateSceneGraph();

        Ogre::Root *root = mGraphicsSystem->getRoot();
        voxelizer =
                new Ogre::VctVoxelizer( Ogre::Id::generateNewId<Ogre::VctVoxelizer>(),
                                        root->getRenderSystem(), root->getHlmsManager(),
                                        true );
        voxelizer->addItem( cornellItem, false );
        voxelizer->addItem( item, false );
        voxelizer->autoCalculateRegion();
        voxelizer->dividideOctants( 1u, 1u, 1u );
        voxelizer->build();

        vctLighting = new Ogre::VctLighting( Ogre::Id::generateNewId<Ogre::VctLighting>(),
                                             voxelizer, true );
        vctLighting->setAllowMultipleBounces( true );
        vctLighting->update( sceneManager, 6u );

        {
            Ogre::HlmsManager *hlmsManager = mGraphicsSystem->getRoot()->getHlmsManager();

            assert( dynamic_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms( Ogre::HLMS_PBS ) ) );
            Ogre::HlmsPbs *hlmsPbs = static_cast<Ogre::HlmsPbs*>( hlmsManager->getHlms(Ogre::HLMS_PBS) );
            hlmsPbs->setVctLighting( vctLighting );
        }

        mGraphicsSystem->getCamera()->setPosition( Ogre::Vector3( 0, 1.8, 2.5 ) );
    }
    //-----------------------------------------------------------------------------------
    void VoxelizerGameState::update( float timeSinceLast )
    {
//        if( mGraphicsSystem->getRenderWindow()->isVisible() )
//        {
//            static int frame = 0;
//            if( frame > 1 )
//                voxelizer->build();
//            ++frame;
//            Ogre::SceneManager *sceneManager = mGraphicsSystem->getSceneManager();
//            vctLighting->update( sceneManager );
//        }
        TutorialGameState::update( timeSinceLast );
    }
}
