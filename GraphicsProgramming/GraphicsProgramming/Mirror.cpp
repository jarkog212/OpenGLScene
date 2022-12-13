#include "Mirror.h"

//prepare stencil, for the mirror
void Mirror::prepareStencil()
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);                                     //clears stencil just in case
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glDisable(GL_DEPTH_TEST);
	mirrorPlane.renderShape();                             //renders mirror into the stencil buffer
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}


//constructor, sets the plane with proper settings 
Mirror::Mirror(Vertex center, Rotator rotation, float sizeX, float sizeY, int resolution)
{
	mirrorPlane = Plane(resolution, resolution, sizeX, sizeY, center);
	mirrorPlane.setRotation(rotation);
}

//render process for the mirror
void Mirror::render()
{
	bool flag = false;                   //flag used to keep the culling if any is enabled
	if (glIsEnabled(GL_CULL_FACE)) {     
		flag = true;
	}
	prepareStencil();                    //sets up the stencil
	
	for (auto it : mirrorObjects) {      //renders all the reflected objects
		it->renderShape(true);
	}
	mirrorPlane.renderShape();           //renders mirror
	glDisable(GL_STENCIL_TEST);          //disables stencil (cleanup fo the next objects)
	if (flag) {
		glEnable(GL_CULL_FACE);
	}
}

void Mirror::update(float dt)
{
	//full transformation matrix
	Matrix4 T;
	//translation matrix to be used (putting a point in th eplane on the origin for reflection)
	Matrix4 translate = { {
			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{-mirrorPlane.getCenter().x,-mirrorPlane.getCenter().y,-mirrorPlane.getCenter().z , 1} } };

	//getting the normal of the plane, stored separately to reduce memory calls
	Vertex normal = mirrorPlane.getWorldNormal();
	//separated the normals so that the equations do not look ugly
	float Na = normal.x;
	float Nb = normal.y;
	float Nc = normal.z;
	//reflection matrix using the formula,
	Matrix4 reflect = { {
		{-pow(Na,2) + pow(Nb,2) + pow(Nc,2) , -2 * Na * Nb , -2 * Na * Nc , 0},
		{-2 * Na * Nb , pow(Na,2) - pow(Nb,2) + pow(Nc,2) , -2 * Nb * Nc , 0},
		{-2 * Na * Nc , -2 * Nb * Nc , pow(Na,2) + pow(Nb,2) - pow(Nc,2) , 0},
		{0 , 0 , 0 , pow(Na,2) + pow(Nb,2) + pow(Nc,2)} } };

	//concatenation of the transformation matrices to get one final one, -1 ensure the translation is reversed again
	T *= translate;
	T *= reflect;
	translate.matrix[3][0] *= -1;
	translate.matrix[3][1] *= -1;
	translate.matrix[3][2] *= -1;
	T *= translate;

	//iterate through all the objects to be reflected
	for (int i = 0; i < originalObjects.size(); i++) {
		if (mirrorObjects[i] == nullptr || originalObjects[i]->hasMoved) {

			//store original shape values so that the dat ais preserved
			Vertex originalCenter = originalObjects[i]->getCenter();
			std::vector<GLfloat> originalPositions = *(originalObjects[i]->getPositions());

			//make a point matrix for the center, so that it can be multiplied by transform
			Matrix4 centerPoint = { {
				{originalCenter.x,originalCenter.y,originalCenter.z,1},
				{0,0,0,0},
				{0,0,0,0},
				{0,0,0,0} } };

			centerPoint *= T;
			centerPoint.normalizeRow(0);
			originalObjects[i]->setCenter({ centerPoint.matrix[0][0],centerPoint.matrix[0][1],centerPoint.matrix[0][2] });

			//change the rotation axes to fit the reflection using the mirror normal
			Matrix4 rotationT;
			rotationT.buildRotationMatrix(180, Na, Nb, Nc);
			*(originalObjects[i]->getAxes()) *= rotationT;

			//og prepare stenciol placement
			//mirroring all the vertices of the object, while preserving the order, flips the normals however
			std::vector<GLfloat>* positions = originalObjects[i]->getPositions();
			for (int j = 0; j < positions->size(); j += 3) {
				Matrix4 concretePoints = { {
					{(*positions)[j],(*positions)[j + 1],(*positions)[j + 2],1},
					{0,0,0,0},
					{0,0,0,0},
					{0,0,0,0} } };

				concretePoints *= reflect; //does not need translation as it is drawn from relative origin of 0,0 hence only reflect is needed

				concretePoints.normalizeRow(0);

				(*positions)[j] = concretePoints.matrix[0][0];
				(*positions)[j + 1] = concretePoints.matrix[0][1];
				(*positions)[j + 2] = concretePoints.matrix[0][2];
				 
			}        
			if (mirrorObjects[i] == nullptr) {                               //first reflection
				ProceduralShape* temp = originalObjects[i]->copySelf();      //get a copy of the object
				temp->releaseShadowPointers();                               //cleans up th epointers in the object
				*mirrorObjects[i] = *temp;                                   //saves the copy value
				delete temp;                                                 //deletes the object
			}
			else {                                                           //if only updating
				*mirrorObjects[i] = *originalObjects[i];                     //overwrite the value
				mirrorObjects[i]->releaseShadowPointers();                   //release the copy's pointers (they point to the same memory as the original which could be trouble)
			}
			originalObjects[i]->setCenter(originalCenter);                   //reverts the original object
			originalObjects[i]->getAxes()->makeWorldAxes();
			*(originalObjects[i]->getPositions()) = originalPositions;
		}
	}
}

//add another object to mirror
void Mirror::addToReflection(ProceduralShape* object)
{
	originalObjects.push_back(std::shared_ptr<ProceduralShape>(object));            //puts the object into the originalObject vector
	object->hasMoved = true;                                                        //ensures that the reflection will get calculated
	mirrorObjects.push_back(std::shared_ptr<ProceduralShape>(object->copySelf()));  //puts a dummy in the mirrorObjects as a temporary dummy
}

//allows for the entire array of objects to be added
void Mirror::addObjectArray(std::vector< std::shared_ptr<ProceduralShape>> objects) 
{
	originalObjects = (objects);
	for (auto it : originalObjects) {                                               //goes through all objects only as a counting measure
		it->hasMoved = true;                                                        //ensures the reflections wil get recalculated
		mirrorObjects.push_back(std::shared_ptr<ProceduralShape>(it->copySelf()));  //puts a dummy into the vector
	}
}
