#pragma once
#include "StaticCamera.h"

//manager  for camera objects, used to better switch between them as well as discern the active one
class CameraManager
{
	Camera* active = nullptr;                                   //pointer to the currently active camera
	vector<Camera*> cameras;                                    //container for cameras
	Input* input;                                               //input pointer
	
public:
	~CameraManager();                                           //destructor
	CameraManager(Input* input);                                //manager requires input at least
	void handleInput(float dt);                                 //handles switching of the cameras as well as calls the active ones handleInput()
	void applyCamera() { active->apply(); };                    //calls active camera apply() 
	void addCamera(Camera* newCam);                             //add the camera to the container 
	void addSkyboxToCamera(int ID, const char* skyBoxAddress);  //calls setupSkybox() for specific cameras nad creates new texture
	void addSkyboxToCamera(int ID, Texture* skyBoxTex);         //calls setupSkybox() for specific cameras nad uses texture already loaded somewhere else
	void addskyboxToAll(Texture* skyBoxTex);                    //calls setupSkybox() for all cameras nad uses texture already loaded somewhere else
	void setCameraFrustum(int ID,FrustumParams f);              //sets the frustum of a specific camera
	void setAllCameraFrustum(FrustumParams f);                  //sets the frustum of all cameras
	void setCameraSpeed(int ID, float speed);                   //sets the camera speed of specific camera
	void setAllCameraSpeed(int ID, float speed);                //sets the cameras speed for all cameras
	Camera** getActivePointer() { return &active; }             //forwards the active pointer, by returning a pointer to it
};

