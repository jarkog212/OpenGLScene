#pragma once
#include "ProceduralShape.h"

//procedurally created disc
class Disc :
    public ProceduralShape
{
    int edges;                                   //resolution of the disc
    void generate() override;                    //generate override
    void render() override;                      //render override

public:
    ~Disc() {};
    Disc(Vertex center, float size, int edges);  //disc requires resolution, position and size (not scale)
    Disc* copySelf() override;                   //create a copy of self dynamically, returns a pointer to that copy, pointer is of the class type
};

