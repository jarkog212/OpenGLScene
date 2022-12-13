#pragma once
#include "Cube.h"

//encapsulates cube shape with skybox settings
class Skybox
{
	Cube* skybox;                                                                 //skybox cube shape
	TEX_wrap_type wrapping[2] = { TEX_wrap_type::Repeat,TEX_wrap_type::Repeat };  //wrapping for textures if any new ones are to be loaded
	Texture* tex;                                                                 //texture pointer

public:
	~Skybox() { delete skybox; }                                                  //destructor to delete the cube
	Skybox(const char* textureAddress, Vertex initPosition);                      //creates a new texture and dummy position
	Skybox(Texture* texture, Vertex initPosition);                                //uses already loaded texture and dummy position
	void repositionSkybox(Vertex position) { skybox->setCenter(position); };      //repositions the skybox
	void render();                                                                //renders the skybox
};

