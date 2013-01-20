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

#include "HMDStereoRender.h"

#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

static const char* vertexShader =
"uniform int left;"
"void main(void)"
"{"
"  vec2 Position;"
"  Position.xy = gl_Vertex.xy;"
"  if (left) Position.x = clamp(Position.x, -1, 0); else Position.x = clamp(Position.x, 0, 1);"
"  gl_Position = vec4(Position.xy, 0.0, 1.0);"
"  gl_TexCoord[0].st = gl_MultiTexCoord0;"
"}";

static const char *fragShader =
"uniform float c;"
"uniform sampler2D tex;"
"void main()"
"{"
"  vec2 normPos = gl_TexCoord[0].st * 2.0 - 1.0;"
"  vec2 newNormPos = vec2(c*normPos[0] / (pow(normPos[1],2.0) + c), c*normPos[1] / (pow(normPos[0],2.0) + c));"
"  newNormPos = (newNormPos + 1.0) / 2.0;"
"  if ((newNormPos.s > 1 || newNormPos.s < 0.0) || (newNormPos.t > 1 || newNormPos.t < 0.0))"
"    gl_FragColor = vec4(0.0,0.0,0.0,1.0);"
"  else"
"    gl_FragColor = texture2D(tex, newNormPos);"
"}";


static void pV(const char* name, const vector3df& v) {
  cout << name << ": " << v.X << ", " << v.Y << ", " << v.Z << endl;
}

HMDStereoRender::HMDStereoRender(IrrlichtDevice *device, int width, int height) 
 : _width(width), _height(height), _hwidth(width>>1)
 {
  // Default values
  _distCallb.c = -8.1;
  _distCallb.left = 0;
  _eyeSeparation = 1.6;
  _FOV = 1.36;

  // Create render textures
  _driver = device->getVideoDriver();
  _renderTexLeft = _driver->addRenderTargetTexture(dimension2d<u32>(_hwidth,_height));
  _renderTexRight = _driver->addRenderTargetTexture(dimension2d<u32>(_hwidth,_height));

  // Create shader material
  _renderMaterial.Wireframe = false;
  _renderMaterial.Lighting = false;
  _renderMaterial.TextureLayer[0].TextureWrapU = ETC_CLAMP;
  _renderMaterial.TextureLayer[0].TextureWrapV = ETC_CLAMP;

  IGPUProgrammingServices* gpu = _driver->getGPUProgrammingServices(); 
  _renderMaterial.MaterialType=(E_MATERIAL_TYPE)gpu->addHighLevelShaderMaterial(vertexShader, "main", EVST_VS_1_1, fragShader, "main", EPST_PS_1_1, &_distCallb, EMT_SOLID, 1);

  _planeVertices[0] = video::S3DVertex(-1.0f, -1.0f, 0.0f,1,1,0, video::SColor(255,0,255,255), 0.0f, 0.0f);
  _planeVertices[1] = video::S3DVertex(-1.0f,  1.0f, 0.0f,1,1,0, video::SColor(255,0,255,255), 0.0f, 1.0f);
  _planeVertices[2] = video::S3DVertex( 1.0f,  1.0f, 0.0f,1,1,0, video::SColor(255,0,255,255), 1.0f, 1.0f);
  _planeVertices[3] = video::S3DVertex( 1.0f, -1.0f, 0.0f,1,1,0, video::SColor(255,0,255,255), 1.0f, 0.0f);

  _planeIndices[0] = 0;
  _planeIndices[1] = 1;
  _planeIndices[2] = 2;
  _planeIndices[3] = 0;
  _planeIndices[4] = 2;
  _planeIndices[5] = 3;

  // Create cameras
  ISceneManager* smgr = device->getSceneManager();

  _cam = smgr->addCameraSceneNode();
}

HMDStereoRender::~HMDStereoRender() {
  
}

float HMDStereoRender::distortion() {
  return _distCallb.c;
}

void HMDStereoRender::setDistortion(float factor) {
  _distCallb.c = factor;
}

float HMDStereoRender::FOV() {
  return _FOV;
}

void HMDStereoRender::setFOV(float FOV) {
  _FOV = FOV;
}

float HMDStereoRender::eyeSeparation() {
  return _eyeSeparation;
}

void HMDStereoRender::setEyeSeparation(float eyeSeparation) {
  _eyeSeparation = eyeSeparation;
}

void HMDStereoRender::drawAll(ISceneManager* smgr) {
  ICameraSceneNode* camera = smgr->getActiveCamera();


  camera->setFOV(_FOV);
  _cam->setFOV(_FOV);

  // cout << ">>>>>>>>>>>>>> CAMERA" << endl;
  // pV("target", camera->getTarget());
  // pV("rot   ", camera->getRotation());
  // pV("pos   ", camera->getPosition());

  vector3df r = camera->getRotation();
  vector3df tx(-_eyeSeparation, 0.0,0.0);
  tx.rotateXZBy(-r.Y);
  tx.rotateYZBy(-r.X);
  tx.rotateXYBy(-r.Z);

  _cam->setPosition(camera->getPosition() + tx);
  _cam->setTarget(camera->getTarget() + tx);

  // cout << ">>>>>>>>>>>>>> TMP_CAMERA" << endl;
  // pV("target", _cam->getTarget());
  // pV("rot   ", _cam->getRotation());
  // pV("pos   ", _cam->getPosition());
  // pV("tx   ", tx);

  _driver->setRenderTarget(_renderTexLeft, true, true, video::SColor(0,0,0,0));
  smgr->setActiveCamera(_cam);
  smgr->drawAll();

  _driver->setRenderTarget(_renderTexRight, true, true, video::SColor(0,0,0,0));
  smgr->setActiveCamera(camera);
  smgr->drawAll();

  _driver->setRenderTarget(0);
  _distCallb.left = 1;
  _renderMaterial.setTexture(0, _renderTexLeft);
  _driver->setMaterial(_renderMaterial);
  _driver->drawIndexedTriangleList(_planeVertices, 4, _planeIndices, 2);

  _distCallb.left = 0;
  _renderMaterial.setTexture(0, _renderTexRight);
  _driver->setMaterial(_renderMaterial);
  _driver->drawIndexedTriangleList(_planeVertices, 4, _planeIndices, 2);
}