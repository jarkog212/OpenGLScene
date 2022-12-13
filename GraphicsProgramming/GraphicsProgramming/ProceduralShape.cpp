#pragma once
#include "ProceduralShape.h"
#include "Light.h"
#include "Plane.h"
#include "Scene.h"

//sets up the vertex into the vertex arrays, colour included
void ProceduralShape::setupVertex(Vertex &ver, float nx, float ny, float nz, float u, float v, Colour colour)
{
	ver.setNormal(nx, ny, nz);                              //sets vertex normal
	ver.setTexCoord(u, v);                                  //sets vertex uvs
	ver.pushVertexToArray(positions, normals, texCoord);    //pusehes the vertex into correct arrays
	colour.pushColourToArray(colours);                      //pushes colour into the array
}

//sets up vertex into the vertex arrays
void ProceduralShape::setupVertex(Vertex& ver, float nx, float ny, float nz, float u, float v)
{
	setupVertex(ver, nx, ny, nz, u, v, DefinedColours::white());     //fills in the colour white for the vertex
}

//creates copies of the object and offsets their center at a given offset
void ProceduralShape::createArrayOfSelf(int num, Vertex offset)
{
	for (int i = 1; i <= num; i++) {                                 //cucles for each new copy
		
		ProceduralShape* temp = copySelf();                          //creates a new copy on the heap 
		temp->setCenter(offset * i);                                 //sets its center to be the relative offset
		temp->setRotation({ 0,0,0 });                                //rotation is local hence its 0
		temp->children.clear();                                      //since the copy has pointers to the same children as parent, delete is not available, clearing is better as it releases those pointers from the copy
		temp->releaseShadowPointers();                               //similar to the above just with shadow pointers
		temp->childWorldOffset = childWorldOffset + center;          //updates the child offset for proper global position (does not take into account rotation, so not very good)
		children.push_back(std::shared_ptr<ProceduralShape>(temp));  //pushes the copy into the children vector
	}
}

//generates the shadow volume objects
void ProceduralShape::generateShadowVolume(Light* light)
{
	Matrix4 F = getRotationMatrix();                              //gets the rotation matrix of this shape
	std::vector<GLfloat> positionsRelative = positions;           //creates a copy of vertex positions (relative)
	Matrix4 tempM;                                                //data handler matrix
	for (int i = 0; i < positionsRelative.size(); i += 3) {       //cycles through all the positions and applies the rotation matrix to get world positions (not good if caster is a child of other objects i think)
		tempM.matrix[0][0] = positionsRelative[i];                //put one point into the data handler, possible update to speed up the program would be to do multiple points at once (Quads vs Tris)
		tempM.matrix[0][1] = positionsRelative[i + 1];            //...
		tempM.matrix[0][2] = positionsRelative[i + 2];            //...
		tempM *= F;                                               //multiplies the points by the rotation
		positionsRelative[i] = tempM.matrix[0][0] + center.x;     //rewrite the copied positions
		positionsRelative[i + 1] = tempM.matrix[0][1] + center.y; //...
		positionsRelative[i + 2] = tempM.matrix[0][2] + center.z; //...
	}

	shadowVolume->clearShapeData();                              //pre-cautionary, clears the data

	std::vector<GLfloat> volumeVerts = Shadow::buildShadowVolume(light->getPosition(), positionsRelative);   //uses the given function(modified) to build the positions for the rest of the shadow volume
	Vertex temp;                                                                                             //data handler
	for (int i = 0; i < volumeVerts.size(); i += 3) {                                                        //cycles through all generated positions
		temp.x = volumeVerts[i];                                                                             //puts the data into the handler
		temp.y = volumeVerts[i + 1];                                                                         //...
		temp.z = volumeVerts[i + 2];                                                                         //...
		shadowVolume->setupVertex(temp, 0, 0, 0, 0, 0, DefinedColours::black());                             //puts the handler data as well as additional information into the shadow volumes vertex arrays
	}
	shadowVolume->setCenter({ 0,0,0 });                                                                      //shadow volume position is relative to the caster, hence it's 0,0,0 to match the caster                                  
}

//destructor deletes shadows and children
ProceduralShape::~ProceduralShape()
{
	delete shadowImpostor;
	delete shadowVolume;
	delete shadowProjectionShape;
	releaseChildrenPointers();             //its smart pointers hence the deletion is not necessary/will be done by the pointer manager
}

// creates a dynamic copy (on the heap)
ProceduralShape* ProceduralShape::copySelf()
{
	ProceduralShape* out = new ProceduralShape();   //creates adummy dynamically (on the heap)
	*out = *this;                                   //copies this objects values into the dummy
	return out;                                     //returns the pointer
}

// gives back a point on the plane that intersect the line form the center of this shape and a plane in given direction
Vertex ProceduralShape::centerToPlane(Vertex direction, Plane* plane)
{
	Vertex floorN = plane->getWorldNormal();                                         //gets the plane normal, needed for plane equation
	Vertex floorP = plane->getWorldTopLeft();                                        //gets the top left of the plane, needed for plan e equation
	Vertex worldCenter = childWorldOffset + center;                                  //gets the world position (in case child, not well implemented)
	float floorD = floorN.x * floorP.x + floorN.y * floorP.y + floorN.z * floorP.z;  //get the D value of d=ax+by+cz where a,b,c is the normal vector
	float Tval = (floorD - floorN.x * worldCenter.x - floorN.y * worldCenter.y - floorN.z * worldCenter.z) / (floorN.x * direction.x + floorN.y * direction.y + floorN.z * direction.z); //get the t value in the line equation from this center and in given direction 
	Tval += -0.005;                                                                  //tiny offset to prevent z fighting
	Vertex out = worldCenter;                                                        //creates a container for output, start at this object's center...             
	direction *= Tval;                                                               //...multiply direction vector by tval to get an exact vector from center to the plane...
	out += direction;                                                                //...adds the direction to the output...
	return out;                                                                      //...returns the output
}

//rewrites the colour array with the new colour
void ProceduralShape::regenerateColours()
{
	for (int i = 0; i < colours.size(); i += 4) {
		colours[i] = colour.red;
		colours[i + 1] = colour.green;
		colours[i + 2] = colour.blue;
		colours[i + 3] = colour.alpha;
	}
}

//simple render call
void ProceduralShape::render()
{
	ArrayRenderer::renderAllOrdered(GL_QUADS, positions, normals, texCoord, colours);
}

//rotates the object but always in global scope, in order of x,y,z
void ProceduralShape::applyRotationInGlobalSpace()
{
	glRotatef(rotation.x, axes.matrix[0][0], axes.matrix[0][1], axes.matrix[0][2]);               //rotate the shape around x axis (defined in the object)
	Matrix4 R;                                                                                    
	R.buildRotationMatrix(rotation.x, axes.matrix[0][0], axes.matrix[0][1], axes.matrix[0][2]);   //build a matrix that reverses the rotation
	axes *= R;                                                                                    //undo the rotation for axes of the object (will have differen tlocal values but globally it goes back)
	glRotatef(rotation.y, axes.matrix[1][0], axes.matrix[1][1], axes.matrix[1][2]);               //rotate the shape around global y axis (locally its something different and the rotation is applied locally)
	R.buildRotationMatrix(rotation.y, axes.matrix[1][0], axes.matrix[1][1], axes.matrix[1][2]);   //build a matrix that reverses the rotation
	axes *= R;                                                                                    //undo the rotation for axes of the object (will have differen tlocal values but globally it goes back)
	glRotatef(rotation.z, axes.matrix[2][0], axes.matrix[2][1], axes.matrix[2][2]);               //rotate the shape around global z axis (locally its something different and the rotation is applied locally) 
	axes.makeWorldAxes();                                                                         //resets the axes of the shape to match their local state
}

//smart pointer ensures the objects are deleted
void ProceduralShape::releaseChildrenPointers()
{
		children.clear();             //clears the only referrence to the pointer managers, triggering their delete procedures
}

//clears the vertex arrays
void ProceduralShape::clearShapeData()
{
	positions.clear();
	normals.clear();
	texCoord.clear();
	colours.clear();
}

//encapsulates the update function for each subclass with base classe's own update
void ProceduralShape::updateShape(float dt)
{
	update(dt);                                                                                       //calls virtual function for update
	if (shadowProjection && relativeLight != nullptr && (relativeLight->hasMoved || hasMoved)) {      //checks whether this casts a shadow projection, whether there is a light pointer, and whether either light or the shape have moved...
		Vertex shadowDir = (center + childWorldOffset) - relativeLight->getPosition();                //...if so, gets the shadow direction vetor...
		shadowDir.normalize();                                                                        //...normalizes it...

		delete shadowProjectionShape;                                                                 //...deletes the previous shadow projection shape...
		shadowProjectionShape = copySelf();                                                           //...copies itself to be the new shadow projection...
		shadowProjectionShape->releaseShadowPointers();                                               //...release ALL pointers to anything else, as to not delete anything important later... 
		shadowProjectionShape->relativeLight = nullptr;
		shadowProjectionShape->relativeFloor = nullptr;
		shadowProjectionShape->material = nullptr;
		shadowProjectionShape->center = centerToPlane(shadowDir, relativeFloor);                      //...snaps the shadow on the plane that receives the shadow...
		Shadow::generateShadowMatrix(shadowProjectionShape->getShadowProjectionMatrix(), relativeLight->getPosition(), relativeFloor->getWorldNormal() * -1, relativeFloor->getWorldTopLeft());  //...generates the transformation matrix to "squish the model correctly"
	}
	if (shadowVolumeState && relativeLight != nullptr && (relativeLight->hasMoved || hasMoved)) {     //check whether this casts shadow volumes, whether there is light associated and whether this or the light has moved...
		generateShadowVolume(relativeLight);                                                          //...if so, generate a new shadow volume object, delete the old one
	}
}

//gives out the concatenation of rotation matrices OpenGL uses to rotate the shape
Matrix4 ProceduralShape::getRotationMatrix()
{
	Matrix4 Rx;
	Rx.buildRotationMatrix(-rotation.x, axes.matrix[0][0], axes.matrix[0][1], axes.matrix[0][2]);  //create a rotation matrix for x axis rotation
	if (!globalRotation) { axes *= Rx; }                                                           //if the rotations are local, then axes need to be udated to reflect the rotation
	Matrix4 Ry;
	Ry.buildRotationMatrix(-rotation.y, axes.matrix[1][0], axes.matrix[1][1], axes.matrix[1][2]);  //create a rotation matrix for y axis (either global or local)rotation
	if (!globalRotation) { axes *= Ry; }                                                           //if the rotations are local, then axes need to be udated to reflect the rotation
	Matrix4 Rz;
	Rz.buildRotationMatrix(-rotation.z, axes.matrix[2][0], axes.matrix[2][1], axes.matrix[2][2]);  //create a rotation matrix for z axis (either global or local)rotation
	Matrix4 F;
	F *= Rx;                              //concatenate the rotations into one transformation matrix
	F *= Ry;                              //...
	F *= Rz;                              //...
	axes.makeWorldAxes();                 //reset the shape axes 
	return F;                             //output the matrix
}

//sets the shadow impostor by using the size value for both dimension, making a square plane
void ProceduralShape::setShadowImpostor(Texture* texture, float size, Light* relativeL, Plane* relativeP, bool useLight)
{
	setShadowImpostor(texture, size, size, relativeL, relativeP, useLight);
}

//sets up shadow impostor
void ProceduralShape::setShadowImpostor(Texture* texture, float sizeX, float sizeY, Light* relativeL, Plane* relativeP, bool useLight)
{
	delete shadowImpostor;                                      //precaution against memory leak, might cause issues if called inapropriately (like after a copySelf function)
	shadowImpostor = nullptr;                                   //gets rid of the dangling pointer
	shadowImpostor = new Plane(1, 1, sizeX, sizeY, { 0,0,0 });  //creates anew dynamic plane
	shadowImpostor->setTexture(texture);                        //sets the texture of the impostor
	shadowImpostor->setTransparency(true);                      //enables blending
	shadowImp = true;                                           //sets the shape as shadow impostor caster
	shadowImpLightDep = useLight;                               //whether the shadow position needs light to be calculated
	relativeLight = relativeL;                                  //sets the light of this shape, CAN be nullptr
	relativeFloor = relativeP;                                  //sets the shadow receiver plane, MUST be a valid object
}

//sets up shadow volume
void ProceduralShape::setShadowVolume(Scene* scene, Light* rL)
{
	shadowVolumeState = true;                          //sets this shape as shadow volume caster
	relativeLight = rL;                                //sets the ESSENTIAL light object 
	this->scene = scene;                               //sets the scene pointer, might update that to a function pointer instead, used for rendering shadow scene
	delete shadowVolume;                               //memory leak precaution
	shadowVolume = nullptr;                            //dangling pointer precaution
	shadowVolume = new ProceduralShape({ 0,0,0 }, 1);  //creates a new shape dynamically
}

//prepares the shape and scene before render, which is virtual, as well as applies other important aspects
void ProceduralShape::renderShape(bool ignoreChildren)
{
	glPushMatrix();

	glTranslatef(center.x, center.y, center.z);                                         //translate the shape to its center (creating a local origin at that position)

	if (globalRotation) {                                                               //based on the type of rotation applies the rotation OpenGL calls
		applyRotationInGlobalSpace();
	}
	else {
		glRotatef(rotation.x, axes.matrix[0][0], axes.matrix[0][1], axes.matrix[0][2]);
		glRotatef(rotation.y, axes.matrix[1][0], axes.matrix[1][1], axes.matrix[1][2]);
		glRotatef(rotation.z, axes.matrix[2][0], axes.matrix[2][1], axes.matrix[2][2]);
	}

	if (transparency) {                                                                 //enables the blending if the flag is set, the blending function needs to be set somewhere else
		glEnable(GL_BLEND);
	}

	if (texture != nullptr) {                                                           //if there is a texture, apply it 
		glEnable(GL_TEXTURE_2D);
		texture->applyTexture();
	}
	else { glDisable(GL_TEXTURE_2D); }                                                  //if not, ENSURE there is not one used

	if (material != nullptr) {                                                          //if there is material, apply it
		material->applyMaterial();
	}
	else {                                                                              //if not, ENSURE vertex colours are used
		glEnable(GL_COLOR_MATERIAL);
	}

	glScalef(scale[0], scale[1], scale[2]);                                             //scale the objects
	render();                                                                           //call virtual render of the shape
	glScalef(1/scale[0], 1/scale[1], 1/scale[2]);                                       //descale the objects, for trnslates and children

	if (!ignoreChildren) {                                                              //if children are to be drawn, render them, still in the current local origin hence their center need to be relative to the parent
		for (auto it : children) {
			it->renderShapeWithShade();
		}
	}

	glPopMatrix();                                                                      //return the to the global origin scope

	if (shadowVolumeState && relativeLight != nullptr) {                                //if this is a shadow volume caster, render the volume
		renderShadowVolume();
	}
	//CLEANUP
	if (material != nullptr) {                                                          //if material was used, reset the "brush" back 
		material->resetToDefault();
	}
	else {
		glDisable(GL_COLOR_MATERIAL);                                                   //otherwise enable the materials back
	}

	if (glIsEnabled(GL_BLEND)) {                                                        //if blending was on disable it
		glDisable(GL_BLEND);
	}

	if (glIsEnabled(GL_TEXTURE_2D)) {                                                   //if the textures are used, disable them
		glDisable(GL_TEXTURE_2D);
	}
}

//render the shape with shadow, can be set to ignotre the children
void ProceduralShape::renderShapeWithShade(bool ignoreChildren)
{
	renderShadows();
	renderShape(ignoreChildren);
}

//render the shadows if availbale
void ProceduralShape::renderShadows()
{
	
	if (relativeFloor != nullptr) {                                                                     //all shadows require a floor hence if not available, skips the entire function
		if (shadowProjection && relativeLight != nullptr && relativeLight->getActive()) {               //if this is shadow projection caster and there is light associated with this shape, and the light is active...
			glDisable(GL_DEPTH_TEST);                                                                   //...disable the depth test...
			renderShadowProjection();                                                                   //...render the shadow projection...
			glEnable(GL_DEPTH_TEST);                                                                    //...reenable the depth test
		}
		if (shadowImp && relativeLight != nullptr && shadowImpLightDep && relativeLight->getActive()) { //if this is a shadow impostor caster, there is a light associated, impotor needs a light and light is active...
			Vertex shadowDir = center - relativeLight->getPosition();                                   //...get the direction from light to this shape...
			shadowDir.normalize();                                                                      //...normalizes this vector...
			renderShadowImpostor(shadowDir);                                                            //...and renders the impostor on the floor using the direction vector
		}
		else if (shadowImp) {                                                                           //if there is an impostor for this object and there is limited number of aspects...
			Vertex shadowDir = relativeFloor->getWorldNormal() * -1;                                    //...use the normal of the floor as direction instead...                                    
			renderShadowImpostor(shadowDir);                                                            //...render the imposto using the direction
		}
	}
}

//renders shadow projection
void ProceduralShape::renderShadowProjection()
{
	
	glDisable(GL_LIGHTING);                                                 //lighting should not affect the shadow
	glDisable(GL_TEXTURE_2D);                                               //shadow objects have no texture, hence it can be disabled
	glEnable(GL_BLEND);                                                     //enables blending for transparency
	glEnable(GL_COLOR_MATERIAL);                                            //enables vertex colours
	setColour({ 0.1f, 0.1f, 0.1f,0.2f });                                   //colour of the shade

	Matrix4* m = shadowProjectionShape->getShadowProjectionMatrix();        //gets the precalculated shadow matrix
	float shadowM[16] = {                                                   //unpacks it into a 1D float array
		m->matrix[0][0],m->matrix[0][1],m->matrix[0][2],m->matrix[0][3],
		m->matrix[1][0],m->matrix[1][1],m->matrix[1][2],m->matrix[1][3],
		m->matrix[2][0],m->matrix[2][1],m->matrix[2][2],m->matrix[2][3],
		m->matrix[3][0],m->matrix[3][1],m->matrix[3][2],m->matrix[3][3] };

	glPushMatrix();                         
	glMultMatrixf((GLfloat*)shadowM);                                       //use OpenGL to flatten the shape
	
	//translate to floor and draw shadow, remember to match any transforms on the object, similar steps to the actual shape render
	glTranslatef(shadowProjectionShape->center.x, shadowProjectionShape->center.y, shadowProjectionShape->center.z);
	if (globalRotation) {
		applyRotationInGlobalSpace();
	}
	else {
		glRotatef(rotation.x, axes.matrix[0][0], axes.matrix[0][1], axes.matrix[0][2]);
		glRotatef(rotation.y, axes.matrix[1][0], axes.matrix[1][1], axes.matrix[1][2]);
		glRotatef(rotation.z, axes.matrix[2][0], axes.matrix[2][1], axes.matrix[2][2]);
	}
	glScalef(scale[0], scale[1], scale[2]);
	render();
	glScalef(1/scale[0], 1/scale[1], 1/scale[2]);
	glPopMatrix();
	setColour({ 1.0f, 1.0f, 1.0f,1.0f });                     //resets the "brush"
	glDisable(GL_COLOR_MATERIAL);                             //disables the vertex colours
	glDisable(GL_BLEND);                                      //disables the blending
	glEnable(GL_LIGHTING);                                    //enables lighting
}

//renders the shadow impostor
void ProceduralShape::renderShadowImpostor(Vertex shadowDir)
{
	bool wasLighting = false;                     //need to disable lighting for this process but might need to have on after, hence the flag is used to keep the setting if the lights were enabled
	if (glIsEnabled(GL_LIGHTING)) {
		wasLighting = true;
		glDisable(GL_LIGHTING);
	}
	shadowImpostor->center = centerToPlane(shadowDir, relativeFloor) - childWorldOffset;  //sets the placement of the impostor
	shadowImpostor->setRotation(relativeFloor->getRotation());                            //matches the floor rotation
	shadowImpostor->renderShape();                                                        //renders the plane
	if (wasLighting) {                                                                    //reenables the lights if available
		glEnable(GL_LIGHTING);
	}
}

//render the shadow volume
void ProceduralShape::renderShadowVolume()
{
	//Set up the setting for stencil
	glClearStencil(0);                          
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //disables colour buffer
	glDepthMask(GL_FALSE);                               //disables the depth

	glEnable(GL_STENCIL_TEST);                           //enables stencil
	glEnable(GL_CULL_FACE);                              //culls faces

	glCullFace(GL_BACK);                                 //cull back faces
	glStencilFunc(GL_ALWAYS, 1, 1);                      //stencil function to simply pass stencil test
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);              //stencil operation sets the value of the buffer to 1 where shape is drawn
	shadowVolume->renderShape();                         //draws the volume front faces

	glCullFace(GL_FRONT);                                //cull front faces
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);              //changes the stencil function to decrease values
	shadowVolume->renderShape();                         //draw the volume back faces

	glDisable(GL_CULL_FACE);                             //disable the culling
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);     //enable colour buffer
	glDepthMask(GL_TRUE);                                //enable depth

	glStencilFunc(GL_EQUAL, 1, 1);                       //sets up the stencil test to only draw within the stencil (values of 1)
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);              //disables the stencil modifications
	glDisable(GL_LIGHTING);                              //disables the lights
	scene->renderShadowScene();                          //render shadow scene, form the scene, can be better with a pointer to the function instead
	glDisable(GL_STENCIL_TEST);                          //disables stencil test
	glEnable(GL_LIGHTING);                               //enables the lighting
}

//render the object in shade
void ProceduralShape::renderInShade(Colour shadow)
{
	Colour temp = colour;                          //stores the original colour to reserve it
	Material* tempM = material;                    //stores the material just to preserve it
	material = nullptr;                            //removes the material to ensure verte colours are drawn instead
	if (shadow.alpha < 1) { glEnable(GL_BLEND); }  //enables the blending ONLY if the shadow colour requires it
	setColour(shadow);                             //sets the colour, therefore recalculates the entire colour array to the shadow
	renderShape();                                 //renders the shape
	setColour(temp);                               //resets the colour
	material = tempM;                              //resets the material
} 
