#include "Cube.h"

//initialises the cube to be ready for the generation of vertices
void Cube::initialise()
{
	float step = size / 2.0f;             //step for each point from center is half of the size in magnitude in every axes

	//creates the points of the cube and sets the positions, this is not veritces just the positions (local scope)
	//point 1 (FF - BL)
	points[0] = { -step, -step, step };

	//point 2 (FF - TL)
	points[1] = { -step, step, step };

	//point 3 (FF - TR)
	points[2] = { step, step, step };

	//point 4 (FF - BR)
	points[3] = { step, -step, step };
	
	//point 5 (BF - BR)
	points[4] = { step, -step, -step };

	//point 6 (BF - BL)
	points[5] = { -step, -step, -step };

	//point 7 (BF - TL)
	points[6] = { -step, step, -step };

	//point 8 (BF - TR)
	points[7] = { step, step, -step };

	generate();
}

// constructors that wach allow for definition of vertex colours for faces or materials and textures
// constructors call the base class constructor to deal with variables properly

Cube::Cube(Vertex center, float size) : ProceduralShape(center,size)
{
	initialise();
}

Cube::Cube(Vertex center, float size, Colour colour1) : ProceduralShape(center, size)
{
	faceColours[0] = colour1;
	initialise();
}

Cube::Cube(Vertex center, float size, Colour colour1, Colour colour2) : ProceduralShape(center, size)
{
	faceColours[0] = colour1;
	faceColours[1] = colour2;
	initialise();
}

Cube::Cube(Vertex center, float size, Colour colour1, Colour colour2, Colour colour3) : ProceduralShape(center, size)
{
	faceColours[0] = colour1;
	faceColours[1] = colour2;
	faceColours[2] = colour3;
	initialise();
}

Cube::Cube(Vertex center, float size, Colour colour1, Colour colour2, Colour colour3, Colour colour4) : ProceduralShape(center, size)
{
	faceColours[0] = colour1;
	faceColours[1] = colour2;
	faceColours[2] = colour3;
	faceColours[3] = colour4;
	initialise();
}

Cube::Cube(Vertex center, float size, Colour colour1, Colour colour2, Colour colour3, Colour colour4, Colour colour5) : ProceduralShape(center, size)
{
	faceColours[0] = colour1;
	faceColours[1] = colour2;
	faceColours[2] = colour3;
	faceColours[3] = colour4;
	faceColours[4] = colour5;
	initialise();
}

Cube::Cube(Vertex center, float size, Colour colour1, Colour colour2, Colour colour3, Colour colour4, Colour colour5, Colour colour6) : ProceduralShape(center, size)
{
	faceColours[0] = colour1;
	faceColours[1] = colour2;
	faceColours[2] = colour3;
	faceColours[3] = colour4;
	faceColours[4] = colour5;
	faceColours[5] = colour6;
	initialise();
}

Cube::Cube(Vertex center, float size, Material* material) : ProceduralShape(center, size, material) 
{
	initialise();
}

Cube::Cube(Vertex center, float size, Texture* texture) : ProceduralShape(center, size, texture)
{
	initialise();
}

//changes the texture coordinates so that all faces show the same area and use the entire UV area
void Cube::setTexCoordsEqual()
{
	texCoord.clear();              //clears original T shaped Texture coordinates
	for (int i = 0; i < 6; i++) {  //loops for every face
		texCoord.push_back(0);     //bottom left point
		texCoord.push_back(1);     //...
		texCoord.push_back(1);     //bottom right point
		texCoord.push_back(1);     //...
		texCoord.push_back(1);     //top right point
		texCoord.push_back(0);     //...
		texCoord.push_back(0);     //top left point
		texCoord.push_back(0);     //...
	}
}

//calls the appropriate array renderer
void Cube::render()
{
	ArrayRenderer::renderAllOrdered(GL_QUADS, positions, normals, texCoord, colours);
}

//creates a dynamic copu on the heap
Cube* Cube::copySelf()
{
	Cube* out = new Cube({0,0,0},1);     //creates a dummy cube on the heap
	*out = *this;                        //copies the values of this cube into the dummy
	return out;                          //returns the pointer to the copy
}

void Cube::generate()
{
	//uses the position points to create vertices, positions repeat but normals and UVs do not, hence 24 vertices need to be made out of the 8 points
	//setup vertex is defined in ProceduralShape and assigns values to the vertex arrays
	//face 1 - front
	setupVertex(points[0], 0, 0, 1, 0.0f, 0.5f, faceColours[0]);
	setupVertex(points[3], 0, 0, 1, 0.25f, 0.5f, faceColours[0]);
	setupVertex(points[2], 0, 0, 1, 0.25f, 0.25f, faceColours[0]);
	setupVertex(points[1], 0, 0, 1, 0.0f, 0.25f, faceColours[0]);

	//face 2 - right
	setupVertex(points[3], 1, 0, 0, 0.25f, 0.5f, faceColours[1]);
	setupVertex(points[4], 1, 0, 0, 0.5f, 0.5f, faceColours[1]);
	setupVertex(points[7], 1, 0, 0, 0.5f, 0.25f, faceColours[1]);
	setupVertex(points[2], 1, 0, 0, 0.25f, 0.25f, faceColours[1]);

	//face 3 - back
	setupVertex(points[4], 0, 0, -1, 0.5f, 0.5f, faceColours[2]);
	setupVertex(points[5], 0, 0, -1, 0.75f, 0.5f, faceColours[2]);
	setupVertex(points[6], 0, 0, -1, 0.75f, 0.25f, faceColours[2]);
	setupVertex(points[7], 0, 0, -1, 0.5f, 0.25f, faceColours[2]);

	//face 4 - left
	setupVertex(points[5], -1, 0, 0, 0.75f, 0.5f, faceColours[3]);
	setupVertex(points[0], -1, 0, 0, 1.0f, 0.5f, faceColours[3]);
	setupVertex(points[1], -1, 0, 0, 1.0f, 0.25f, faceColours[3]);
	setupVertex(points[6], -1, 0, 0, 0.75f, 0.25f, faceColours[3]);

	//face 5 - bottom
	setupVertex(points[5], 0, -1, 0, 0.0f, 0.75f, faceColours[4]);
	setupVertex(points[4], 0, -1, 0, 0.25f, 0.75f, faceColours[4]);
	setupVertex(points[3], 0, -1, 0, 0.25f, 0.5f, faceColours[4]);
	setupVertex(points[0], 0, -1, 0, 0.0f, 0.5f, faceColours[4]);

	//face 6 - top
	setupVertex(points[1], 0, 1, 0, 0.0f, 0.25f, faceColours[5]);
	setupVertex(points[2], 0, 1, 0, 0.25f, 0.25f, faceColours[5]);
	setupVertex(points[7], 0, 1, 0, 0.25f, 0.0f, faceColours[5]);
	setupVertex(points[6], 0, 1, 0, 0.0f, 0.0f, faceColours[5]);
}




