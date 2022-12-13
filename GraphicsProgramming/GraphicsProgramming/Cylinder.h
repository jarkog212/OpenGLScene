#pragma once
#include "ProceduralShape.h"
#include "Disc.h"
#include <assert.h> 

//class building a cylinder procedurally
class Cylinder : public ProceduralShape
{
    float height;                        //the world height of the cylinder(length in relative y-axis )
    int levels;                          //resolution of the relative y-axes
    int facesPerRow;                     //vertical resolution
    Disc bottom = Disc({ 0,0,0 },0,0);   //The predefined disc shape to be put on top, resolution matches vertical resolution
    Disc top = Disc({ 0,0,0 }, 0, 0);    //The predefined disc shape to be put on bottom, resolution matches vertical resolution
    void generate() override;            //generate override
    void render() override;              //render override

public:
    ~Cylinder();
    Cylinder(Vertex center, float size, float height, int levels, int facesPerRow) : ProceduralShape(center, size) {       //each cylinder requires height, and resolutions as well as position and size (not scale)
        this->levels = levels;                                                                                             
        this->facesPerRow = facesPerRow; 
        this->height = height;
        generate();}
    Cylinder* copySelf() override;                           //creates a copy of self dynamically and returns a pointer to the copy, pointer is of the class type
    Disc* getTopDisc() { return &top; }                      //returns a pointer to the top disc object
    Disc* getBottomDisc() { return &bottom; }                //returns a pointer to the bottom disc object
};

