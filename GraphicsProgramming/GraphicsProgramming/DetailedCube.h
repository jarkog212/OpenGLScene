#pragma once
#include "ProceduralShape.h"
#include "Plane.h"

//cube objects with detailed sides uses planes to generate sides
class DetailedCube : public ProceduralShape
{
	Plane sides[6];                                           //six plane objects to be used to calculate cube vertices
	int resolution = 1;                                       //resolution of each plane
	void generate() override;                                 //generate override
	void render() override;                                   //render override
public:
	~DetailedCube() {};
	DetailedCube(Vertex center, float size, int resolution);  //Detailed cube needs resolution as well as position and size
	Plane* getSide(int index);                                //returns a reference to a side with proper offset, used for shadows
	DetailedCube* copySelf() override;                        //creates a copy of itself dynamically, returns a pointer to that copy, pointer is of the class type 
};

