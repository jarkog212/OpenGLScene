#pragma once
#include "Cube.h"

//enum used to differentiate between light types as they do not have separate subclasses
enum class LightType { Ambient_Only, Ambient_Directional , Ambient_Point , Directional_Only , Point_Only , Spot};

//class that handles and sends light information to OpenGL
class Light {

	LightType type;                                         //type of light stored
	bool isOn = false;                                      //the flag to specify whether the individual light is to be applied or not
	double lightID = 0x4000;                                //light ID, uses deprecated version as values (was not aware of GLenum type and reprogramming was not necessary for functionality)
	GLfloat spotCutoff = 35;                                //self evident, used for spotlights
	GLfloat spotExponent = 20;                              //self evident, used for spotlights
	GLfloat Direction[4] = { 0.0f,0.0f,0.0f,0.0f };         //Direction parameter for light (if the last value is 1 it becomes position)
	GLfloat Spot_Direction[4] = { 0.0f, -1.0f, 0.0f,1.0 };  //Direction for spotlight
	GLfloat Ambient_Colour[4] = { 0.1f,0.1f,0.1f,1.0f };    //Ambient parameter for light
	GLfloat Diffuse_Colour[4] = { 1.0f,1.0f,1.0f,1.0f };    //Diffuse parameter for light
	Cube* debugCube;                                        //Cube object to be placed at the light's position if needed
	Rotator rotation;                                       //orientation of the lights (used with glRotate())

public:
	bool hasMoved = true;                                                          //optimization flag, whenever moved it gets set to true
	Light(LightType type, int lightID, Vertex posDir, bool visibleSource);         //constructor, lights require type, ID (0-7), position/direction, use of debug cube or not specified
	void applyLight();                                                             //calls appropriate OpenGL functions to set lights using its parameters
	void toggleLight();                                                            //activates, deactivates light
	void moveLight(Vertex offset);                                                 //changes the position of light according to the offset
	void orbitRotateLight(Rotator rotation);                                       //adds the rotation to light's rotation, around local origin (by default its equal to global)
	void orbitSetAngleLight(Rotator rotation);                                     //sets the rotation of light, around local origin (by default its equal to global)
	//accessors
	void setDiffuseColour(Colour colour);                                          
	bool getActive() { return isOn; }                                               
	void setAmbientColour(Colour colour);
	void setSpotAtributes(float cutoff, float exponent, Vertex spotDirection);   
	void setAttenuation(float constant, float linear, float quadratic);
	Vertex getPosition();
	Vertex getDirection(Vertex objectPos);
};

