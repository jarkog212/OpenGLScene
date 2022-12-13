// Model class, represents a loaded OBJ model
// handles the loading/processing of the model file. Also loads and stores its own texture
#ifndef _MODEL_H_
#define _MODEL_H_

// INCLUDES //
#include <fstream>


using namespace std;

// MY CLASS INCLUDES //
//#include <vector>
#include "Vector3.h"
//#include "SOIL.h"
#include "ProceduralShape.h"

//clas sthat handles external mnodels, load them and manages them. Is based on ProceduralShape
class Model : public ProceduralShape
{

	void render() override;               //render function override
	void loadTexture(char*);              //creates a new texture and assigns it to the shape
	bool loadModel(char*);                //loads the model and translates it to vertex arrays
public:
	~Model() {};
	Model(Vertex center, float size) : ProceduralShape(center, size) {};       //model requires position and size(scale)
	bool load(char* modelFilename, char* textureFilename);                     //encapsulates functions for texture and model into one call
	bool load(char* modelFilename);                                            //just loads the model with translation to vertex array 
	bool load(char* modelFilename, bool TXCOORD_flipYaxis);                    //similar to before but flips all texture cordinates on y axis - programs like blender have TX origin at bottom left
	void generate() override {};                                               //generate function override
	Model* copySelf() override;                                                //creates a copy of self dynamically and returns a pointer to it, pointer is of the class type 
};

#endif