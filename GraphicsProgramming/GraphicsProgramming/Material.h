#pragma once

#include <cstdlib>
#include <string>
#include "glut.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Structs.h"

//material class, holds all the parameters and function calls for OpenGL
class Material
{
	std::string name;                                 //name of the material, not really needed but still implemented
	GLfloat Diffuse[4] = {0.6f,0.6f,0.6f,1.0f};       //Diffuse colour parameter of material (array)
	GLfloat Ambient[4] = { 0.1f,0.1f,0.1f,1.0f };     //Ambient colour parameter of material (array)
	GLfloat Emission[4] = { 0.0f,0.0f,0.0f,1.0f };    //Emission colour parameter of material (array)
	GLfloat Specular[4] = { 1.0f,1.0f,1.0f,1.0f };    //Specular colour parameter of material (array)
	int shininess = 50;                               //shininess colour parameter of material
	float emissionMultiplier = 1;                     //emission multiplier, used to multiply the emission colours to schieve more "bright" colour
	double face = 0x0404;                             //used for doublesidedness (deprecated as I was not aware of GLenum at the time, there was no nbeed to replace it)

public:
	Material() {};                                                                                                              //default material constructor
	Material(std::string name, bool twoSided);                                                                                  //better default constructor, still needs name and the twosidedness specified
	Material(std::string name, bool twoSided, Colour Diffuse, Colour Ambient, Colour Emission, Colour Specular, int shininess); //best constructor, sets all parameters immidiately and makes material ready to be used

	void applyMaterial();                  //calls repsective OpenGL functions to apply material
	void resetToDefault();                 //resets the OpenGL material "brush" to default, used to not propagate the material when not needed

	// setters
	void setEmissionMultiplier(float multiplier);
	void setDiffuse(Colour diffuse);
	void setDiffuseAndAmbient(Colour diffuse);
	void setAmbient(Colour ambient);
	void setEmission(Colour emission);
	void setSpecular(Colour specular);
	void setShininess(int shininess);

	//getters
	float getEmissionMultiplier() { return emissionMultiplier; };
	Colour getDiffuse() { Colour out = { Diffuse[0],Diffuse[1],Diffuse[2],Diffuse[3] }; return out; }
	Colour getAmbient() { Colour out = { Ambient[0],Ambient[1],Ambient[2],Ambient[3] }; return out; }
	Colour getEmission() { Colour out = { Emission[0],Emission[1],Emission[2],Emission[3] }; return out; }
	Colour getSpecular() { Colour out = { Specular[0],Specular[1],Specular[2],Specular[3] }; return out; }
	int getShininess() {  return shininess; }
};

