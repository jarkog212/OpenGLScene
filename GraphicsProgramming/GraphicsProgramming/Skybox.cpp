#include "Skybox.h"

//constructor that creates the new texture and then forwards the parameters 
Skybox::Skybox(const char* textureAddress, Vertex initPosition) : Skybox(new Texture(TEX_application_type::Modulate, wrapping, TEX_filtering_type::Point, textureAddress),initPosition)
{
}

//constructor that uses the already loaded texture
Skybox::Skybox(Texture* texture, Vertex initPosition)
{
	tex = texture;
	skybox = new Cube(initPosition, 1);           //new cube is created on the heap
	skybox->setRotation({ 0,0,180 });
	skybox->setTexture(tex);
}

//render requires abit of a setup
void Skybox::render()
{
	bool wasCulling = false;             //keeps track of culling state 
	bool wasLighting = false;            //keeps track of lighting state
	glDisable(GL_DEPTH_TEST);            //disables depth test
	if (glIsEnabled(GL_CULL_FACE)) {     //disables culling if it was enabled, remembers this
		glDisable(GL_CULL_FACE);        
		wasCulling = true;
	}
	if (glIsEnabled(GL_LIGHTING)) {      //disables lights, remembers this
		glDisable(GL_LIGHTING);
		wasLighting = true;
	}

	skybox->renderShape();               //renders the cube

	glEnable(GL_DEPTH_TEST);             //resets the depth test
	if (wasCulling) {                    //resets the culling if available
		glEnable(GL_CULL_FACE);
	}
	if (wasLighting) {                   //resets the lights if available
		glEnable(GL_LIGHTING);
	}
}
