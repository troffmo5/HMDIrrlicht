/*
Copyright (C) 2012 Luca Siciliano

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies 
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <irrlicht.h>
#include "HMDStereoRender.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Configuration
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 800;
bool fullscreen = false;
bool vsync = true;
float eyeSeparation = 5.6;
float FOV = 1.04;
float mouseSpeed = 40.0f;
float walkSpeed = 3.0f;
float distortion = -8.1;

class MyEventReceiver : public IEventReceiver
{
public:
   bool OnEvent(const SEvent& event)
   {
      if (event.EventType == EET_MOUSE_INPUT_EVENT) {
        if (event.MouseInput.Event == EMIE_MOUSE_MOVED) {
          vector2df pos = cursor->getRelativePosition();
          if (pos.X != 0.5f || pos.Y != 0.5f) {
            vector3df rot = renderer->egoRotation();
            
            rot.Y += (pos.X - 0.5f) * mouseSpeed;
            if (rot.Y < 0) rot.Y += 360;
            if (rot.Y > 360) rot.Y -= 360;

            rot.X -= (pos.Y - 0.5f) * mouseSpeed;
            if (rot.X < 0) rot.X += 360;
            if (rot.X > 360) rot.X -= 360;
            
            renderer->setEgoRotation(rot);
            cursor->setPosition(0.5f, 0.5f);
           }

        }
        
      }
     
      if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown) {
        if (event.KeyInput.Key == KEY_ESCAPE)
        {
          device->closeDevice();
          return true;
        }    
        else if (event.KeyInput.Key == KEY_UP) {
          vector3df delta(0.0f, 0.0f, -walkSpeed);
          vector3df rot = renderer->egoRotation();
          delta.rotateYZBy(rot.X);
          delta.rotateXZBy(-rot.Y);
          renderer->setEgoPosition(renderer->egoPosition()+delta);
          return true;
        }
        else if (event.KeyInput.Key == KEY_DOWN) {
          vector3df delta(0.0f, 0.0f, walkSpeed);
          vector3df rot = renderer->egoRotation();
          delta.rotateYZBy(rot.X);
          delta.rotateXZBy(-rot.Y);
          renderer->setEgoPosition(renderer->egoPosition()+delta);
          return true;
        }
        else if (event.KeyInput.Key == KEY_LEFT) {
          vector3df delta(walkSpeed, 0.0f, 0.0f);
          vector3df rot = renderer->egoRotation();
          delta.rotateYZBy(rot.X);
          delta.rotateXZBy(-rot.Y);
          renderer->setEgoPosition(renderer->egoPosition()+delta);
          return true;
        }
        else if (event.KeyInput.Key == KEY_RIGHT) {
          vector3df delta(-walkSpeed, 0.0f, 0.0f);
          vector3df rot = renderer->egoRotation();
          delta.rotateYZBy(rot.X);
          delta.rotateXZBy(-rot.Y);
          renderer->setEgoPosition(renderer->egoPosition()+delta);
          return true;
        }
        else if (event.KeyInput.Key == KEY_KEY_A) {
          vector3df delta(0.0f, walkSpeed, 0.0f);
          vector3df rot = renderer->egoRotation();
          delta.rotateYZBy(rot.X);
          delta.rotateXZBy(-rot.Y);
          renderer->setEgoPosition(renderer->egoPosition()+delta);
          return true;
        }
        else if (event.KeyInput.Key == KEY_KEY_Z) {
          vector3df delta(0.0f, -walkSpeed, 0.0f);
          vector3df rot = renderer->egoRotation();
          delta.rotateYZBy(rot.X);
          delta.rotateXZBy(-rot.Y);
          renderer->setEgoPosition(renderer->egoPosition()+delta);
          return true;
        }
        else if (event.KeyInput.Key == KEY_KEY_S) {
          renderer->setEyeSeparation(renderer->eyeSeparation() + 0.2);
          printf("eye separation: %f\n", renderer->eyeSeparation());

          return true;
        }
        else if (event.KeyInput.Key == KEY_KEY_X) {
          renderer->setEyeSeparation(renderer->eyeSeparation() - 0.2);
          printf("eye separation: %f\n", renderer->eyeSeparation());
          return true;
        }
        else if (event.KeyInput.Key == KEY_KEY_F) {
          renderer->setFOV(renderer->FOV() + 0.005);
          printf("FOV: %f\n", renderer->FOV());
          return true;
        }
        else if (event.KeyInput.Key == KEY_KEY_V) {
          renderer->setFOV(renderer->FOV() - 0.005);
          printf("FOV: %f\n", renderer->FOV());
          return true;
        }
        else if (event.KeyInput.Key == KEY_KEY_D) {
          renderer->setDistortion(renderer->distortion() + 0.1);
          printf("distortion: %f\n", renderer->distortion());
          return true;
        }
        else if (event.KeyInput.Key == KEY_KEY_C) {
          renderer->setDistortion(renderer->distortion() - 0.1);
          printf("distortion: %f\n", renderer->distortion());
          return true;
        }        
      }
      return false;
   }
   IrrlichtDevice* device;
   HMDStereoRender* renderer;
   ICursorControl* cursor;
};


int main(){
  MyEventReceiver receiver;
  IrrlichtDevice *device = createDevice(EDT_OPENGL, dimension2d<u32>(SCREEN_WIDTH, SCREEN_HEIGHT), 16, fullscreen, false, vsync, &receiver);
  receiver.device = device;
  receiver.cursor = device->getCursorControl();

  IVideoDriver* driver = device->getVideoDriver();
  ISceneManager* smgr = device->getSceneManager();
  IGUIEnvironment* guienv = device->getGUIEnvironment();
  IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();

  HMDStereoRender renderer(device, SCREEN_WIDTH, SCREEN_HEIGHT);
  receiver.renderer = &renderer;
  renderer.setEyeSeparation(eyeSeparation);
  renderer.setFOV(FOV);
  renderer.setDistortion(distortion);

  // Create world

  // load the quake map
  device->getFileSystem()->addZipFileArchive("./map-20kdm2.pk3");
  IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
  ISceneNode* levelNode = smgr->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 128);
  levelNode->setPosition(core::vector3df(-1350,-90,-1400));

  // load a faerie 
  IAnimatedMesh* faerie = smgr->getMesh("./faerie.md2");
  IAnimatedMeshSceneNode* faerieNode = smgr->addAnimatedMeshSceneNode(faerie);
  faerieNode->setMaterialTexture(0, driver->getTexture("./faerie2.bmp"));
  faerieNode->setMaterialFlag(EMF_LIGHTING, false);
  faerieNode->setPosition(vector3df(40,190,-1030));
  faerieNode->setRotation(vector3df(0,-90,0));
  faerieNode->setMD2Animation(EMAT_SALUTE);

  // load a dwarf
  IAnimatedMesh* dwarf = smgr->getMesh("./dwarf.x");
  IAnimatedMeshSceneNode* dwarfNode = smgr->addAnimatedMeshSceneNode(dwarf);
  dwarfNode->setPosition(vector3df(40,-25,20));

  device->getCursorControl()->setVisible(false);

  // Render loop
  while(device->run()){
    driver->beginScene(true,true,SColor(0,100,100,100));

    renderer.drawAll(smgr);
   
    // end scene
    driver->endScene();
  }
  device->drop();
  return 0;
}
