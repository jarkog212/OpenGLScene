#include "Sphere.h"

//generates the sphere using athematical formula for circle and loops
void Sphere::generate()
{
	float theta = 2.0f * PI / sectorsPerRow;          //angle between each vertex on the edge of the circle, resolution dependent
	float alpha = 0;                                  //lattitude angle
	float beta = PI / 2.0f;                           //longitude angle

	for (int i = 0; i <= sectorsPerRow/2; i++) {      //cycle through rows...
		for (int j = 0; j <= sectorsPerRow; j++) {    //...and collumns 
			float x = cos(alpha) * cos(beta);         //use 3D sphere formula for positions
			float y = sin(beta);                      //...
			float z = sin(alpha) * cos(beta);         //...
			Vertex temp = {                           //data handler
				x,                                    //positions
				y,                                    //...
				z,                                    //...
				x,                                    //normals, matching the positions in unit sphere
				y,                                    //...
				z,                                    //...
				float(float(j) / sectorsPerRow),      //texture coords, spread out like a map and globe
				float(float(i) / (sectorsPerRow/2))   //...
			};

			temp.pushVertexToArray(positions, normals, texCoord);   //pushes the vertex to appropriate arrays
			colour.pushColourToArray(colours);                      //pushes the colour into the colour array

			alpha += theta;                                         //add the angle for next vertex
		}
		alpha = 0;                                    //reset angle to 0 for next "level" 
		beta -= theta;                                //go one "level lower" to draw the next circle
	}

	//calculate the indices so that the vertices can be reused
	for (int i = 0; i < sectorsPerRow/2; i++) {                             
		for (int j = 0; j < sectorsPerRow; j++) {
			indices.push_back((i * (sectorsPerRow+1)) + j);              //top left of a face
			indices.push_back((i * (sectorsPerRow+1)) + (j + 1));        //top right of a face
			indices.push_back((i + 1) * (sectorsPerRow+1) + (j + 1));    //bottom right of a face
			indices.push_back((i + 1) * (sectorsPerRow+1) + j);          //bottom left of a face
		}
	}
}

//calls the right array renderer
void Sphere::render()
{
	glPushMatrix();
	glScalef(size,size,size);    // circle size is its scaling
	ArrayRenderer::renderAllIndexedOrder(GL_QUADS, positions, normals, texCoord, indices);
	glPopMatrix();
}

//creates a copy of self dynamically 
Sphere* Sphere::copySelf()
{
	Sphere* out = new Sphere({0,0,0}, 1, 1);  //create a dummy dynamically (on the heap)
	*out = *this;                             //copy this object's value into the dummy
	return out;                               //return the pointer
}
