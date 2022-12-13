#include "CameraManager.h"

//deletes all cameras as all of them are dynamically created
CameraManager::~CameraManager()
{
	for (auto it : cameras) {
		delete it;             //deletes the camera
		it = nullptr;          //dangling pointer prevention
	}
	cameras.clear();           //clears the vector
}

// sets up the input pointer
CameraManager::CameraManager(Input* input)
{
	this->input = input;
}

//handles and forwards input function calls
void CameraManager::handleInput(float dt)
{
	if(input->isKeyDown('q')) {                                    //if Q is pressed...
		active = cameras[(active->getID() + 1) % cameras.size()];  //set the next camera from this one as the active one
		input->setKeyUp('q');        
	}
	active->handleInput(dt);                                       //calls only the active camera's handle input
}

//adds a new camera to the manager
void CameraManager::addCamera(Camera* newCam)
{
	if (cameras.size() == 0) {       //if there are no prior cameras, the new added one is first and hence the only active one
		active = newCam;
	}
	cameras.push_back(newCam); 
	newCam->setupInput(input);       //sets the camera input
}

//calls the setup for a specific camera in the manager, creates a new exture 
void CameraManager::addSkyboxToCamera(int ID, const char* skyBoxAddress)
{
	if (ID < cameras.size()) {
		cameras[ID]->setupSkybox(skyBoxAddress);
	}
}

//calls the setup for a specific camera in the manager, uses already loaded texture 
void CameraManager::addSkyboxToCamera(int ID, Texture* skyBoxTex)
{
	if (ID < cameras.size()) {
		cameras[ID]->setupSkybox(skyBoxTex);
	}
}

//calls the setup for all cameras and only allows to use already loaded texture (otherwise memory intensive)
void CameraManager::addskyboxToAll(Texture* skyBoxTex)
{
	for (auto it : cameras) {
		it->setupSkybox(skyBoxTex);
	}
}

//sets the frustum of specific camera, uses frustum struct defined in camera
void CameraManager::setCameraFrustum(int ID, FrustumParams f)
{
	if (ID < cameras.size()) {
		cameras[ID]->redoFrustum(f.top,f.bottom,f.right,f.left,f.Fnear,f.Ffar);
	}
}

//sets the frustum of all the cameras, uses frustum struct defined in camera
void CameraManager::setAllCameraFrustum(FrustumParams f)
{
	for (auto it : cameras) {
		it->redoFrustum(f.top, f.bottom, f.right, f.left, f.Fnear, f.Ffar);
	}
}

//changes the camera movement speed, not sensitivity of a specific camera
void CameraManager::setCameraSpeed(int ID, float speed)
{
	if (ID < cameras.size()) {
		cameras[ID]->setCameraSpeed(speed);
	}
}

//changes the camera movement speed of all camers, not sensitivity
void CameraManager::setAllCameraSpeed(int ID, float speed)
{
	for (auto it : cameras) {
		it->setCameraSpeed(speed);
	}
}
