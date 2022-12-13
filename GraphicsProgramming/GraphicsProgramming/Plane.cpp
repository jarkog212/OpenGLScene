#pragma once
#include "Plane.h"
#include "Light.h"

//constructor, invokes base construcot where base classes size vatiable is arbitrarily filled
Plane::Plane(int rows, int columns, float sizeX, float sizeY, Vertex center) : ProceduralShape(center, 1)
{
	//dimensions and resolution to  be used for generating the plane
	sectors[0] = columns;            
	sectors[1] = rows;
	size[0] = sizeX;
	size[1] = sizeY;
	generate();
}

//Limited in number of sectors, i.e. must be a square number or it will downscale, deprecated
Plane::Plane(int sectors, float sizeX, float sizeY, Vertex center) : ProceduralShape (center,1)
{
	this->sectors[0] = sqrt(sectors);
	this->sectors[1] = this->sectors[0];
	size[0] = sizeX;
	size[1] = sizeY;
	generate();
}

//calculates the normal in global scale
Vertex Plane::getWorldNormal()
{
	Vertex out;                     //container for output
	Matrix4 T;                      //transformation matrix
	Matrix4 originalNormal = {{     //plane is calculated by having a starting normal of 0,1,0
		{0,1,0,1},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}} };

	originalNormal *= getRotationMatrix();        //uses the rotations stored to calculate how the OpenGL rotates the shape, applying that to the original normal
	//taking the points out of the matrix into the output
	out.x = originalNormal.matrix[0][0];         
	out.y = originalNormal.matrix[0][1];
	out.z = originalNormal.matrix[0][2];

	return out;
}

//gets the top left most point of the plane in global scope
Vertex Plane::getWorldTopLeft()
{
	Vertex worldCenter = childWorldOffset + center;                                                    //uses the parent offsets, if child object to add to the center
	Matrix4 F;                                                                                         //final transformation matrix
	Matrix4 T1 = { {                                                                                   //translate, we need d value of 0, or in other words a point of the plane to cross global origin...
			{1,0,0,0},                                                                                 //...hence, we only know one point for sure that globally lies on the plane, the center which it's drawn from
			{0,1,0,0},
			{0,0,1,0},
			{-worldCenter.x,-worldCenter.y,-worldCenter.z , 1} } };
	F *= T1;                                                                                           //concatenate matrix with the final matrix
	F *= getRotationMatrix();                                                                          //gets the actual rotation matrix to be used by OpenGL to rotate the object, concatenate it with final matrix
	T1.matrix[3][0] *= -1;                                                                             //inverts the translate as...
	T1.matrix[3][1] *= -1;      
	T1.matrix[3][2] *= -1;
	F *= T1;                                                                                           //...we need to translate the plane back, concatenates with final matrix, creating the final transformation matrix 

	Matrix4 P = {{                                                                                     //point matrix, position needs to be in this format to allow matrix multiplication
			{worldCenter.x + positions[0],worldCenter.y + positions[1],worldCenter.z + positions[2],1},
			{0,0,0,0},
			{0,0,0,0},
			{0,0,0,0} } };

	P *= F;                                                                                            //applies the final transformation matrix to the point matrix

	//axes.makeWorldAxes(); //?
	Vertex out = { P.matrix[0][0]*scale[0],P.matrix[0][1]*scale[1],P.matrix[0][2]*scale[2] };          //rolls out the result into a Vertex format
	return out;                                                                                        //to return it out
}

//plane generation process
void Plane::generate()
{
	positions.clear();               //pre-emptive cleaning
	normals.clear();                 //...
	colours.clear();                 //...
	texCoord.clear();                //...

	Vertex topLeft;                  //local top left point of the face to be generated
	topLeft.x = -size[0] / 2;        //starts at the complete top left of the plane (which is half the entire size to the left from origin)
	topLeft.y = 0;                   //local y coordinate will always be 0 as plane is 2D 
	topLeft.z = -size[1] / 2;        //starts at the complete top left of the plane (which is half the entire size to the top from the origin)
	topLeft.ny = 1.0f;               //all vertices have a normal of 0,1,0

	Vertex currentVertex = topLeft;  //uses current vertex to handle data

	float columnSectorDistance = size[0] / sectors[0];  //column width given the complete size and resolution of the plane
	float rowSectorDistance = size[1] / sectors[1];     //row height given the complete height and resolution of the plane

	for (int i = 0; i < sectors[0]; i++) {                                   //generates points for each face in a subcycle (and row in a cycle) (multiple in the same position, mostly because final and first vertices of each point share position)      
		for (int j = 0; j < sectors[1]; j++) {

			currentVertex = { topLeft.x + j * columnSectorDistance,          //start at the current face top left, defines position... normal remains, and uvs are similar just scaled to fit 0-1 range
							 0,
							 topLeft.z + i * rowSectorDistance,
							 0,                                              //...normal remains 0,1,0...
							 1,
							 0,
							 j* (1.0f / sectors[0])* UVmultipliers[0],       //... and uvs are similar just scaled to fit 0-1 range
							 i* (1.0f / sectors[1])* UVmultipliers[1], };

			currentVertex.pushVertexToArray(positions, normals, texCoord);   //pushes the top left vertex of a face into vertex arrays
			colour.pushColourToArray(colours);                               //adds colour just in case

			currentVertex.z += rowSectorDistance;                            //moves the point
			currentVertex.UV_v = (i+1)*(1.0f/sectors[1]) * UVmultipliers[1]; //moves the uvs

			currentVertex.pushVertexToArray(positions, normals, texCoord);   //pushes bottom left vertex of a face into vertex arrays
			colour.pushColourToArray(colours);                               //colour just in case

			currentVertex.x += columnSectorDistance;                         //moves the postition
			currentVertex.UV_u = (j+1)*(1.0f/sectors[0]) * UVmultipliers[1]; //moves the uvs

			currentVertex.pushVertexToArray(positions, normals, texCoord);   //pushes bottom right vertex of a face into vertex arrays
			colour.pushColourToArray(colours);                               //colour just in case

			currentVertex.z -= rowSectorDistance;                            //moves tho position
			currentVertex.UV_v = i * (1.0f/sectors[1]) * UVmultipliers[1];   //moves the uvs

			currentVertex.pushVertexToArray(positions, normals, texCoord);   //pushes the top right vertex of a face into vertex arrays
			colour.pushColourToArray(colours);                               //colour just in case
		}
	}
}

//renders all vertices as ordered, considering there are copies of the same positions
void Plane::render()
{
	ArrayRenderer::renderAllOrdered(GL_QUADS, positions, normals, texCoord, colours);
}

//creates dynamic copy (on the heap)
Plane* Plane::copySelf()
{
	Plane* out = new Plane(1, 1, 1, {0,0,0});   //creates adynamic dummy on the heap
	*out = *this;                               //copies this object values into the dummy
	return out;                                 //returns its pointer
}


