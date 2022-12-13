#pragma once
#include "Plane.h"
#include "Cube.h"

//class for billboards, based off of plane

class Camera;                   //dummy class due to the includes
class Billboard : public Plane
{
	Camera** activeCamera;             //pointer to the active camera pointer, used for matching the rotation
	void rotateToCamera();             //function that sets rotation to match the camera
	void update(float dt) override;    //update override

public:
	~Billboard();
	Billboard(Vertex center, float size) : Plane(1, 1, size, size, center) { setRotation({ 90,0,0 }); globalRotation = true; }                    //constructor with same sides for plane
	Billboard(Vertex center, float sizeX, float sizeY) : Plane(1, 1, sizeX, sizeY, center) { setRotation({ 90,0,0 }); globalRotation = true; }    //constructor with different sides for plane
	Billboard* copySelf() override;                                                                                                               //copySelf override to return a pointer of this class's type
	void setActiveCamera(Camera** activeCamera) { this->activeCamera = activeCamera; }                                                            //sets the pointer, used for initialisation
}; 

