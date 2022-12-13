#pragma once
#include "Structs.h"
#include "Shadow.h"
#include "Material.h"

//dummy classes needed for pointers
class Plane;
class Light;
class Scene;

//base class for any renderable shape
class ProceduralShape
{
protected:
	bool transparency = false;                          //whether to enable blending or not
	bool shadowImp = false;                             //whether the shape has shadow impostor or not
	bool shadowImpLightDep = false;                     //whether the light is necessary for shadow impostor placement
	bool shadowProjection = false;                      //whether shape has shadow projection (3D planar shadow)
	bool shadowVolumeState = false;                     //whether shape has volumetric shadow
	bool globalRotation = false;                        //whether to rotate this object in its local axes or global axes
	float size = 1;                                     //size (not scale usually) of the object
	Vertex center;                                      //position of origin in global scope
	Plane* relativeFloor = nullptr;                     //plane to be used as a floor for shadows
	Light* relativeLight = nullptr;                     //light to be used to calculate the shadow from (except impostor which is not dependednt but may be compliant)
	float scale[3] = { 1,1,1 };                         //scale modifier
	Colour colour;                                      //Vertex colour of the entire object
	Rotator rotation = { 0,0,0 };                       //Rotation of the object
	Plane* shadowImpostor = nullptr;                    //pointer to the shadow impostor plane
	ProceduralShape* shadowProjectionShape = nullptr;   //pointer to the shadow projection shape
	ProceduralShape* shadowVolume = nullptr;            //pointer to the shadow Volume
	Scene* scene = nullptr;                             //pointer to the scene, used by shadopw volume to render "shadow world"
	Material* material = nullptr;                       //pointer to the shape material
	Texture* texture = nullptr;                         //pointer to the shape texture
	Matrix4 projectionMatrixShadow;                     //transformation matrix used for shadow projection
	Matrix4 axes = { {                                  //local axes of the objects tracker
		{1,0,0,1},
		{0,1,0,1},
		{0,0,1,1},
		{0,0,0,1}} };

	std::vector<GLfloat> positions;                          //vertex array, holds position of vertices in local scope
	std::vector<GLfloat> normals;                            //vertex array, holds normals of vertices in local scope
	std::vector<GLfloat> texCoord;                           //vertex array, holds UVs of vertices 
	std::vector<GLfloat> colours;                            //vertex array, holds vertex colours  
	std::vector<GLuint> indices;                             //vertex array, holds the order of vertices to be drawn (not all subclasses use this)

	std::vector<std::shared_ptr<ProceduralShape>> children;  //vector of shapes to be tied to this shape, rendering gets called before the maatrix stack pop is called hence is used to have hierarchical transforms

	virtual void generate() {};                              //virtual generate function, used for procedural generation of shape vertices (depends on shape)
	virtual void generateShadowVolume(Light* light);         //generates the shape for procedural volume and stores it 

	Vertex centerToPlane(Vertex direction, Plane* plane);    //gets the coordinates of a point moved along normal on the plane
	void regenerateColours();                                //recalculates the entire colour array to have new values
	virtual void render();                                   //virtual render function, each shape migjht have small differences hence is virtual
	void applyRotationInGlobalSpace();                       //rotate the object in global axes always, in x,y,z order
	void releaseChildrenPointers();                                   //deletes the children of the shape
	void renderShadowProjection();                           //renders shadow projection
	void renderShadowImpostor(Vertex shadowDir);             //renders shadow impostor in a given direction
	void renderShadowVolume();                               //renders the shadow volume
	virtual void update(float dt) {};                        //update to be used 

public:
	bool hasMoved = true;                                                                                                                                      //flag for optimization, notes whther the position of this object has changed
	Vertex childWorldOffset = { 0,0,0 };                                                                                                                       //tracker for the parents position, not done too well but can be used to track global position
	ProceduralShape() {};                                                                                                                                      //dummy constructor
	virtual ~ProceduralShape();                                                                                                                                //virtual destrucotr for easier disposal of objects
	ProceduralShape(Vertex center, float size) { this->center = center; this->size = size; }                                                                   //base constructor, objects should be set up with size and position 
	ProceduralShape(Vertex center, float size, Texture* texture) : ProceduralShape(center, size) { this->texture = texture; };                                 //constructor that allows for texture to be set up as well
	ProceduralShape(Vertex center, float size, Material* material) : ProceduralShape(center, size) { this->material = material; };                             //constructor that allows for material to be set up as well
	ProceduralShape(Vertex center, float size, Texture* texture, Material* material) : ProceduralShape(center, size, texture) { this->material = material; };  //constructor that allows for both texture and material to be set up
	virtual ProceduralShape* copySelf();                                                                                                                       //creates a copy of self dynamically and returns a pointer to self, made virtual so that the return typ can be children class
	//accessors
	Vertex getCenter() { return center; };                                             
	Rotator getRotation() { return rotation; }
	void setScale(float sx, float sy, float sz) {scale[0] = sx; scale[1] = sy; scale[2] = sz; hasMoved = true;};
	void setCenter(Vertex center) { this->center = center; hasMoved = true; }
	void setColour(Colour colour) { this->colour = colour; regenerateColours(); hasMoved = true; };
	void setTransparency(bool transparency) { this->transparency = transparency; };
	void setMaterial(Material* material) { this->material = material; hasMoved = true; };
	void setTexture(Texture* texture) { this->texture = texture; hasMoved = true; };
	void rotate(Rotator rotation) { this->rotation += rotation; hasMoved = true; };                                                 //adds rotation angle to the current one
	void setRotation(Rotator rotation) { this->rotation = rotation; hasMoved = true; };                                             //sets the rotation angle
	void setShadowImpostor(Texture* texture, float size, Light* relativeL, Plane* relativeP, bool useLight = false);                //sets up and creates the shadow impostor, texture needs to be loaded beforehand
	void setShadowImpostor(Texture* texture, float sizeX, float sizeY, Light* relativeL, Plane* relativeP, bool useLight = false);  //sets up and creates the shadow impostor with variable dimensions, texture needs to be loaded beforehand
	void setShadowProjection(Light* rL, Plane* rP) { shadowProjection = true; relativeLight = rL; relativeFloor = rP; }             //sets up for shadow projection shape to be generated, assigns all prerequisite pointers
	void setShadowVolume(Scene* scene, Light* rL);                                                                                  //sets up for shadow volume to be generated, assigns all prerequisite pointers
	void setGlobalRotation(bool state) { globalRotation = state; }                                         
	void addChild(ProceduralShape* newChild) { children.push_back(std::shared_ptr<ProceduralShape>(newChild)); newChild->childWorldOffset = childWorldOffset + center; };  //adds a child object to the vector, also sets the world offset
	void setupVertex(Vertex& ver, float nx, float ny, float nz, float u, float v, Colour colour);                        //function to easily set up a vertex, uses the positions and given normals,UVs and colours and puts them to appropriate arrays
	void setupVertex(Vertex& ver, float nx, float ny, float nz, float u, float v);                                       //function to easily set up a vertex, uses the positions and given normals,UVs and puts them to appropriate arrays
	void createArrayOfSelf(int num, Vertex offset);                                                                      //creates desired number of copies, sabes them as children with slight offsets to their positions
	void clearShapeData();                                                                                               //clears all vertex arrays
	void updateShape(float dt);                                                                                          //updates the procedural shape stuff and calls virtual update function
	Matrix4* getShadowProjectionMatrix() { return &projectionMatrixShadow; }                         
	Matrix4 getRotationMatrix();                                                                                         //calculates and returns the matrix that open gl used to rotate the object, used for transitions between local and global scope
	ProceduralShape* getChild(int i) { return children[i].get(); }
	std::vector<GLfloat>& getTexCoords() { return texCoord; }                                                            //returns a refference to the TX array, used to make changes if needed
	std::vector<GLfloat>* getPositions() { return &positions; }                                                          //returns a pointer to the positions, used heavily in transitions form local to global scope or reflections
	Matrix4* getAxes() { return &axes; }                                                              
	void renderShape(bool ignoreChildren = false);                                                                       //renders the shape with all the transforms but allows to skip children, used in optimizations
	void renderShapeWithShade(bool ignoreChildren = false);                                                              //encapsulates the rendering process with shadow rendering 
	void renderShadows();                                                                                                //renders only shadows, if available
	void renderInShade(Colour shadow);                                                                                   //renders darkened version of self, used if object is rendered within volumetric shadow
	void releaseShadowPointers() { shadowImpostor = nullptr; shadowVolume = nullptr; shadowProjectionShape = nullptr; }; //very dangerous and prone to memory leak, use with caution
};
