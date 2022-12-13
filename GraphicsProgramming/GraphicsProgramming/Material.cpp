#include "Material.h"

//bare-bones constructor, sets up bare minimum for a material. Should not be used
Material::Material(std::string name, bool twoSided)
{
	this->name = name;
	if (twoSided) {
		face = 0x0408;
	}
}

//constructor, sets up material attributes by forwarding the parameters to functions
Material::Material(std::string name, bool twoSided, Colour Diffuse, Colour Ambient, Colour Emission, Colour Specular, int shininess) : Material (name,twoSided) {
	setDiffuse(Diffuse);
	setAmbient(Ambient);
	setEmission(Emission);
	setSpecular(Specular);
	setShininess(shininess);
}

// calls the appropriate OpenGl funcitons using the defined setting
void Material::applyMaterial()
{
	glMaterialfv(face, GL_AMBIENT, Ambient);
	glMaterialfv(face, GL_DIFFUSE, Diffuse);
	glMaterialfv(face, GL_SPECULAR, Specular);
	glMateriali(face, GL_SHININESS, shininess);
	glMaterialfv(face, GL_EMISSION, Emission);
	glDisable(GL_COLOR_MATERIAL);               //precautionary measure
	
}

// redefines the material "brush" in OpenGL to the default setting, used to not propagate material to objects that dont have them
void Material::resetToDefault()
{
	GLfloat defAmbient[4] = { 0.2,0.2,0.2,1 };   //base OpenGL ambient
	GLfloat defDiffuse[4] = { 0.8,0.8,0.8,1 };   //base OpenGL diffuse
	GLfloat defSpecular[4] = { 0,0,0,1 };        //base OpenGL specular
	GLfloat defEmission[4] = { 0,0,0,1 };        //base OpenGL emission 

	glMaterialfv(face, GL_AMBIENT, defAmbient);
	glMaterialfv(face, GL_DIFFUSE, defDiffuse);
	glMaterialfv(face, GL_SPECULAR, defSpecular);
	glMateriali(face, GL_SHININESS, 0);
	glMaterialfv(face, GL_EMISSION, defEmission);
	glDisable(GL_COLOR_MATERIAL);                //precautionary measure
}

//emmision multiplier set
void Material::setEmissionMultiplier(float multiplier)
{
	emissionMultiplier = multiplier;             
	for (int i = 0; i < 4; i++) {                       //multiplies the emissive colour values
		Emission[i] *= multiplier;
	}
}

//sets the diffuse colour
void Material::setDiffuse(Colour diffuse)
{
	this->Diffuse[0] = diffuse.red;
	this->Diffuse[1] = diffuse.green;
	this->Diffuse[2] = diffuse.blue;
	this->Diffuse[3] = diffuse.alpha;
}

//sets both diffuse and ambient colour
void Material::setDiffuseAndAmbient(Colour diffuse)
{
	this->Diffuse[0] = diffuse.red;
	this->Diffuse[1] = diffuse.green;
	this->Diffuse[2] = diffuse.blue;
	this->Diffuse[3] = diffuse.alpha;

	this->Ambient[0] = diffuse.red;
	this->Ambient[1] = diffuse.green;
	this->Ambient[2] = diffuse.blue;
	this->Ambient[3] = diffuse.alpha;
}

//sets the ambient colour
void Material::setAmbient(Colour ambient)
{
	this->Ambient[0] = ambient.red;
	this->Ambient[1] = ambient.green;
	this->Ambient[2] = ambient.blue;
	this->Ambient[3] = ambient.alpha;
}

//sets the emission colour
void Material::setEmission(Colour emission)
{
	this->Emission[0] = emission.red;
	this->Emission[1] = emission.green;
	this->Emission[2] = emission.blue;
	this->Emission[3] = emission.alpha;
}

//sets the colour of the specular reflection
void Material::setSpecular(Colour specular)
{
	this->Specular[0] = specular.red;
	this->Specular[1] = specular.green;
	this->Specular[2] = specular.blue;
	this->Specular[3] = specular.alpha;
}

//sets the shininess
void Material::setShininess(int shininess)
{
	if (shininess > 128) {       //shininess is clamped
		shininess = 128;
	}
	else if (shininess < 0) {
		shininess = 0;
	}
	this->shininess = shininess;
}

