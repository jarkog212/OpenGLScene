#pragma once
#define PI 3.14159
#define TO_RADIANS (PI / 180)

#include "Skybox.h"
#include "Vector3.h"
#include "Input.h"

//struct used for frustum params to be grouped together
struct FrustumParams {
	float bottom;
	float top;
	float right;
	float left;
	float Fnear;
	float Ffar;
};

//class dealing with camera and proper parsing of values to OpenGL's functions
class Camera
{
protected:                              
	int ID = 0;                        //ID used for camera manager index
	bool active_controls = true;       //flag used for dettaching the mouse from the camera
	int* screen_width;                 //pointer to scene's screensize variable
	int* screen_height;                //pointer to scene's screensize variable
	float rotation_sensitivity = 0.2;  //degrees per mouse pixel
	float cameraSpeed = 10;            //camera speed
	FrustumParams frustum;             //frustum data
	Skybox* skybox;                    //pointer to skybox, pointer as not every camera has to have a skybox
	Vector3 position;                  //positions vector for camera
	Vector3 RelativeForward;           //camera's forward vector
	Vector3 RelativeUp;                //camera's up vector
	Vector3 RelativeRight;             //camera's right vector
	Rotator rotation;                  //camera's rotation in roll, pitch, yaw
	Input* input;                      //input pointer form scene

	void calculateCamera();            //calculates the camera settings, called within update 
	void applyCamera();                //calls gluLookAt() function with cameras variables

public:
	Camera(Vertex position,Rotator rotation,int ID,int* screen_width,int* screen_height);                               //initialises the camera, requirements exist for every camera hence no default contructor 
	void redoFrustum(float new_Top, float new_Bottom, float new_Right, float new_Left, float new_Near, float new_Far);  //changes the frustum settings and saves them

	//accessors
	FrustumParams getFrustum() { return frustum; }
	Vector3 getPosition() { return position; }
	Vector3 getRelativeForward() { return RelativeForward; }
	Vector3 getRelativeUp() { return RelativeUp; }
	Vector3 getRelativeRight() { return RelativeRight; }
	Rotator getRotation() { return rotation; }
	int getID() { return ID; }
	void rotateCamera(Rotator rotation);
	void setupInput(Input* in) { input = in; }
	void setCameraSpeed(float speed) { cameraSpeed = speed; }
	void setDimensions(int* width, int* height) { screen_width = width; screen_height = height; }

	void apply();                                   //bundles the functions to be run in the correct order to render                  
	void moveForward(float speed);                  //adds to the position in forward direction at given offset multiplier
	void moveRight(float speed);                    //adds to the position in right direction at given offset multiplier (strafing)
	void moveUp(float speed);                       //adds to the position in upward direction at given offset multiplier
	void mouseControl();                            //uses mouse positions and center of the screen to rotate camera
	virtual void handleInput(float dt);             //controls setup, virtual to allow many types of cameras
	void setupSkybox(const char* textureAddress);   //sets up skyboox objects, creates new texture using the address
	void setupSkybox(Texture* texture);             //sets up skyboox objects, uses already loaded texture
}; 

