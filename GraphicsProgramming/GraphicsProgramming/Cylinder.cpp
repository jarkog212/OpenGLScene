#include "Cylinder.h"

//procedural generation of cylinder veritces
void Cylinder::generate()
{
	top = Disc({ 0,height / 2.0f,0 }, size, facesPerRow);     //the discs are the top...
	bottom = Disc({ 0,-height / 2.0f,0 }, size, facesPerRow); //...and bottom of the shape
	top.setRotation({180,0,0});                               //flips the top disc to point outward	
	float currentHeight = height / 2.0f;                      //current height is used to determine the local y coordinate from which to generate a circle
	float deltaHeight = height / float(levels);               //how much to decrease y coordinate to draw the next circle, dependent on resolution
	float theta = 2.0f * PI / float(facesPerRow);             //angle between each vertex in the circle
	
	for (int j = 0; j <= levels; j++) {                       //generate a circle for every level
		for (int i = 0; i <= facesPerRow; i++) {              //generate one vertex in a circle per face, and add one at the end
			Vertex temp = {
				size/2 * cos(i * theta),                      //x position of the vertex using a circle formula
				currentHeight,                                //y is the current level y coordinate
				-size/2 * sin(i * theta),                     //z position of the vertex using a circle formula
				cos(i * theta),                               //normals match the position in unit circle 
				0,                                            //normals lay on xy axis
				-sin(i * theta),                              //normals match the position in unit circle 
				float(float(i) / float(facesPerRow)) / 2.0f,          //divides the entire UV square into chunks and slowly adds to draw arolled out cylinder on a plane, takes only half of vertical UVs to fit the disc uvs
				-(currentHeight + (height / 2.0f) - height) / height  //divides the entire UV square into chunks and slowly adds to draw arolled out cylinder on a plane, adjusts the y to math the UV model,
			};
			temp.pushVertexToArray(positions, normals, texCoord);
			colour.pushColourToArray(colours);
		}
		currentHeight -= deltaHeight;                         //move one level down
	}

	//calculates the indices, using the proper render order for a quad and moves the indices by offsets
	for (int i = 0; i < levels; i++) {
		for (int j = 0; j < facesPerRow; j++) {
			indices.push_back(((i + 1) * (facesPerRow + 1)) + j);          //top right of a face
			indices.push_back(((i + 1) * (facesPerRow + 1)) + (j + 1));    //bottom right of a face
			indices.push_back((i * (facesPerRow + 1)) + (j + 1));          //bottom left of a face
			indices.push_back((i * (facesPerRow + 1)) + j);                //top left of a face
		}
	}

	//shidts around th disc UVs to the bottom right of the UV space and scals them to half
	for (int i = 0; i < top.getTexCoords().size(); i++)
	{
		top.getTexCoords()[i] = top.getTexCoords()[i] / 2 + 0.5f;
	}
	for (int i = 0; i < bottom.getTexCoords().size(); i++)
	{
		bottom.getTexCoords()[i] = bottom.getTexCoords()[i] / 2 + 0.5f;
	}
}

//renders everything in proper order, including the discs on top and bottom
void Cylinder::render()
{
	
	ArrayRenderer::renderAllIndexedOrder(GL_QUADS, positions, normals, texCoord, indices);
	top.renderShape();
	bottom.renderShape();
}

Cylinder::~Cylinder()
{
}

//creates a copy dynamically, on the heap
Cylinder* Cylinder::copySelf()
{
	Cylinder* out = new Cylinder({0,0,0}, 1, 1, 1, 1); //creates adummy cylinder in the heap
	*out = *this;                                      //copies values of this cylinder into the dummy
	return out;                                        //returns a dummy
}


