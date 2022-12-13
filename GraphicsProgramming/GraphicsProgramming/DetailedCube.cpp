#include "DetailedCube.h"

//uses the planes of set resolution to generate faces of the cube
void DetailedCube::generate()
{
	Matrix4 points;       //for transformation purposes
	for (int i = 0; i < 6; i++) {                                    //use each side to get the vertices
		std::vector<GLfloat> container = *sides[i].getPositions();   //store all the generated plane vertices for manipulation
		std::vector<GLfloat> containerTX = sides[i].getTexCoords();  //store all the generated UVs for manipulation
		Vertex normal = sides[i].getWorldNormal();                   //store the planes global normal
		for (int j = 0; j < container.size(); j += 3) {              //for every vertex's value (groups of 3) 

			//fill the matrix
			points.matrix[0][0] = container[j];
			points.matrix[0][1] = container[j + 1];
			points.matrix[0][2] = container[j + 2];

			//transform the points according to the plane rotation, changes the local plane positions to local cube positions and store them
			points *= sides[i].getRotationMatrix();                              //rotate the points            

			positions.push_back(points.matrix[0][0] + normal.x * size / 2);      //offset the points to their correct cube-relative positions 
			positions.push_back(points.matrix[0][1] + normal.y * size / 2);      //...
			positions.push_back(points.matrix[0][2] + normal.z * size / 2);      //...

			normals.push_back(normal.x);                                         //normals match the plane
			normals.push_back(normal.y);                                         //...
			normals.push_back(normal.z);                                         //...

			//keeps the plane uvs 
			texCoord.push_back(containerTX[j - (j / 3)]);                        //need to convert a 3 per 1 system into 2 per one system              
			texCoord.push_back(containerTX[(j + 1) - (j / 3)]);                  //...

			colours.push_back(1);                                                //makes vertices white
			colours.push_back(1);
			colours.push_back(1);
			colours.push_back(1);
		}
	}
}

//calls the appropriate vertex array renderer
void DetailedCube::render()
{
	ArrayRenderer::renderAllOrdered(GL_QUADS, positions, normals, texCoord, colours);
}

//creates acopy of self dynamically (on the heap)
DetailedCube* DetailedCube::copySelf()
{
	DetailedCube* out = new DetailedCube({0,0,0},1,1);   //creates a dummy detailed cube
	*out = *this;                                        //copies the values of this cube nto the dummy
	return out;                                          //returns the pointer
}

//constructor, calls the base constructor with th values
DetailedCube::DetailedCube(Vertex center, float size, int resolution) : ProceduralShape(center, size)
{
	this->resolution = resolution;                                      

	// saves copies of the plane for each side with modification to their rotation
	Plane temp = Plane(resolution, resolution, size, size, { 0,0,0 });
	temp.childWorldOffset = center + childWorldOffset;                  //saves the copy's world location, though the faces are not not moved from origin so they dont actually form a cube 
	temp.setGlobalRotation(true);                                       //uses the world rotation system
	sides[0] = temp;                                                    //top face
	temp.setRotation({ 90, 0, 0 });
	sides[1] = temp;                                                    //back face
	temp.setRotation({ 90, 90, 0 });
	sides[2] = temp;                                                    //left face
	temp.setRotation({ 90, 180, 0 });                               
	sides[3] = temp;                                                    //front face
	temp.setRotation({ 90, 270, 0 });
	sides[4] = temp;                                                    //right face
	temp.setRotation({ 180, 0, 0 });
	sides[5] = temp;                                                    //bottom face
	 
	generate();
	
}

//returns a faithfull recreation of the side of the cube
Plane* DetailedCube::getSide(int index)
{
	Matrix4 scaling{ {
		{scale[0],scale[1],scale[2],1},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}} };
	scaling *= sides[index].getRotationMatrix();                                                    //rotates the sciling to match the rotations of the object
	Vertex scalingMultiplier = { scaling.matrix[0][0],scaling.matrix[0][1],scaling.matrix[0][2] };  
	Plane* out = new Plane();                                                                       //creates a new pointer to the floor (dangerous, need to think about how to handle deletion)
	*out = sides[index];                                                                            //copies the plane data 
	Vertex normal = out->getWorldNormal();                                                          //gets the plane normal and...
	normal.x *= scalingMultiplier.x;                                                                //multiplies it by scaling
	normal.y *= scalingMultiplier.y;
	normal.z *= scalingMultiplier.z;
	out->setCenter(out->getCenter() + (normal * (size/2)));                                         //positions the plane ot the correct cube-relative place
	return out;                                                                                     //returns the pointer
}
