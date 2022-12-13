#pragma once
#include "ProceduralShape.h"
// Sphere object procedurally generated
class Sphere :
    public ProceduralShape
{
    int sectorsPerRow;           //resolution for the sphere
    void generate() override;    //generate override 
    void render() override;      //render override
  
public:
    Sphere(Vertex center, float size, float sectorsPerRow) : ProceduralShape(center, size) { this->sectorsPerRow = sectorsPerRow; generate(); };                                       //contructor for sphere, requires resolution
    Sphere(Vertex center, float size, Colour colour, float sectorsPerRow) : ProceduralShape(center, size) { this->sectorsPerRow = sectorsPerRow; this->colour = colour; generate(); }; //same as before but allows for colour to be specified
    Sphere* copySelf() override;    //creates a copy of self dynamically, returns a pointer to it, pointer is of class type
};

