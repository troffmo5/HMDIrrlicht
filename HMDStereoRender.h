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

#ifndef __HMD_STEREO_RENDERER__
#define __HMD_STEREO_RENDERER__

#include <irrlicht.h>

class distorsionCallback: public irr::video::IShaderConstantSetCallBack 
{ 
public:
  float c;
  virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) 
  { 
    irr::video::IVideoDriver* driver = services->getVideoDriver();
    services->setPixelShaderConstant("c", reinterpret_cast<irr::f32*>(&c),1);
  }
};

class HMDStereoRender
{
public:
  HMDStereoRender(irr::IrrlichtDevice *device, int width, int height);
  ~HMDStereoRender();

  float distortion();
  void setDistortion(float factor);

  float FOV();
  void setFOV(float FOV);

  float eyeSeparation();
  void setEyeSeparation(float eyeSeparation);

  irr::core::vector3df egoPosition();
  void setEgoPosition(irr::core::vector3df pos);

  irr::core::vector3df egoRotation();
  void setEgoRotation(irr::core::vector3df rot);

  void drawAll(irr::scene::ISceneManager* smgr);


protected:
  void updateTargets();

private:
  int _width;
  int _hwidth;
  int _height;
  irr::video::IVideoDriver* _driver;
  irr::video::ITexture* _renderTexLeft;
  irr::video::ITexture* _renderTexRight;
  distorsionCallback _distCallb;
  irr::video::SMaterial _renderMaterial;
  irr::video::S3DVertex _planeVertices[4];
  irr::u16 _planeIndices[6];
  irr::scene::ISceneNode* _egoNode;
  irr::scene::ICameraSceneNode* _camLeft;
  irr::scene::ICameraSceneNode* _camRight;
  irr::scene::ISceneNode* _targetLeft;
  irr::scene::ISceneNode* _targetRight;
  float _eyeSeparation;
  float _FOV;
};

#endif
