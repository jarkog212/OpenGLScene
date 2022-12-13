#pragma once
#include <stdio.h>
#include <cassert>
#include "ProceduralShape.h"

//simple cube shape generates vertices using 8 key points
class Cube : public ProceduralShape
{
	Vertex points[8];               //key points that define the cube                 
	Colour faceColours[6];          //colours for each face of the cube (not materials)     

	void initialise();              //since there is a lot of constructors, helper function that initialises the cube was created
	void generate() override;       //override of generate() which generates and saves the positions, normals, texture coordinates and colours
	void render() override;         //render override, has one line call to static struct arrayRender with the type of rendering being quads 
	
public:
	//constructors to specify colour for faces
	~Cube() {};
	Cube(Vertex center, float size);
	Cube(Vertex center, float size, Colour colour1);
	Cube(Vertex center, float size, Colour colour1, Colour colour2);
	Cube(Vertex center, float size, Colour colour1, Colour colour2, Colour colour3);
	Cube(Vertex center, float size, Colour colour1, Colour colour2, Colour colour3, Colour colour4);
	Cube(Vertex center, float size, Colour colour1, Colour colour2, Colour colour3, Colour colour4, Colour colour5);
	Cube(Vertex center, float size, Colour colour1, Colour colour2, Colour colour3, Colour colour4, Colour colour5, Colour colour6);
	Cube(Vertex center, float size, Material* material);
	Cube(Vertex center, float size, Texture* texture);
	Cube* copySelf() override;        //copies itself and returns a pointer of the class type
	void setTexCoordsEqual();         //changes the texture coordinates to have all faces the same, instead of "unfolded"
};

