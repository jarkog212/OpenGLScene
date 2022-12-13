#pragma once

#include "Plane.h"
#include "Cube.h" 

//mirror object, uses plane to represent itself and renders and stores the objects it needs to reflect
class Mirror
{                               
	Plane mirrorPlane;                                                //Plane shape to be rendered as a mirror
	std::vector<std::shared_ptr<ProceduralShape>> originalObjects;    //smart pointers to original objects that need to be reflected 
	std::vector<std::shared_ptr<ProceduralShape>> mirrorObjects;      //smart pointers to copies of the original objects that are reflected
	void prepareStencil();                                            //sets up the stencil buffer

public:
	~Mirror() {};
	Mirror(Vertex center, Rotator rotation, float sizeX, float sizeY,int resolution);    //Constructor, mirror requires position, rotation, plane dimensions and resolution specified 
	void render();                                                                       //bundle of proper functions that need to be called to render mirror
	void update(float dt);                                                               //update to check which objects need to be recalculated for their reflection 
	void addToReflection(ProceduralShape* object);                                       //adds an object to the originalObjects array
	void addObjectArray(std::vector< std::shared_ptr<ProceduralShape>> objects);         //allows for the entire original array to be added to reflection
	Plane* getMirrorPlane() { return &mirrorPlane; };                                    //returns the pointer to the plane
};

