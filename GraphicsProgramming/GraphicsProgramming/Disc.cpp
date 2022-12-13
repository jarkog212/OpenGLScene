#include "Disc.h"

//procedurally generates the the disc using the unit circle equation
void Disc::generate()
{
	float theta = 2.0f * PI / edges;           //the angle between each vertex ont he edge of the shape
	float radius = size / 2.0f;                //radius of the circle is half its size
	Vertex temp;                               //temporary container
	Vertex center;                             //center point

	//sets up the vertex (in the array its the vertex 0)
	setupVertex(center, 0, 1, 0, 0.5, 0.5, DefinedColours::white());  //normal points up (y axis is 1, and UVs are on the center of the UV plane)

	//for every edge point on the circle
	for (int i = 0; i < edges; i++) {
		temp.setPosition(radius * cos(i * theta), 0, -radius * sin(i * theta));                                        //sets the center of the point 
		setupVertex(temp, 0, 1, 0, (cos(i * theta) / 2) + 0.5, (sin(i * theta) / 2) + 0.5, DefinedColours::white());   //setsup the vertex, adds it to the array

		indices.push_back(0);                                                                                          //pushes the central point index...
		if (i == edges - 1) { indices.push_back(1); }                                                                  //...catches the out of bounds index...
		else { indices.push_back((i + 2)); };                                                                          //...pushes the scond vertex index...
		indices.push_back((i + 1));                                                                                    //...pushes the last vertex creating a tri.
		
	}
}

//constructor, calls the base class as well
Disc::Disc(Vertex center, float size, int edges) : ProceduralShape(center,size)
{
	this->edges = edges;
	generate();
}

//uses renderer with correct settings
void Disc::render()
{
	ArrayRenderer::renderAllIndexedOrder(GL_TRIANGLES, positions, normals, texCoord, colours, indices);
}

//creates the copy on the heap
Disc* Disc::copySelf()
{
	Disc* out = new Disc({0,0,0}, 1, 1);   //creates a dummy disc dynamically, on the heap
	*out = *this;                          //copies the values for this disk to the dummy
	return out;                            //returns the dummy
}
