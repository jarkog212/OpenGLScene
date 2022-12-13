#pragma once
#include "ProceduralShape.h"
#include <cmath>

//plane shape procedurally generated
class Plane : public ProceduralShape
{
	int sectors[2];                       //resolution of the plane of both dimensions
	float size[2];                        //size (not scale) of the plane in both dimensions
	float UVmultipliers[2] = { 1,1 };     //multipliers for TX to be used to better manipulate how the texture is shown on the plane
	void generate() override;             //generate override
	void render() override;               //render override
	
public:
	Plane() {};                                                                                                                                   //dummy construcor
	Plane(int rows, int columns, float sizeX, float sizeY, Vertex center);                                                                        //Planes can be initialised with rows, collumns,individual size for each dimension, position specified
	Plane(int sectors, float sizeX, float sizeY, Vertex center);                                                                                  //Plane can be specified in the same way as above except just one resolution is used - deprecated
	virtual Plane* copySelf() override;                                                                                                           //Creates a copy of self dynamically and returns pointer to it, pointer is of the class type
	void setUVmultiplier(float Umultiplier, float Vmultiplier) { UVmultipliers[0] = Umultiplier; UVmultipliers[1] = Vmultiplier; generate(); };   //changes the UVs according to multiplier, recalculates theplane to make changes
	Vertex getWorldNormal();                                                                                                                      //returns a normal vector in global scope
	Vertex getWorldTopLeft();                                                                                                                     //returns a point position of the plane in global scope (arbitrarily top left)
};

