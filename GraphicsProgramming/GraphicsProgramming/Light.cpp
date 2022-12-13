#include "Light.h"

//constructor, based on light type initialises the proper settings
Light::Light(LightType type, int lightID, Vertex posDir, bool visibleSource)
{
	this->type = type;
	this->lightID += lightID;
	// applies special settings for different types
	switch (type) {
		case LightType::Ambient_Point:      //Ambient Point light has a position
			Direction[3] = 1.0f;
			break;
		case LightType::Point_Only:         //lone point light has no ambient light
			Direction[3] = 1.0f;
			Ambient_Colour[0] = 0.0f;
			Ambient_Colour[1] = 0.0f;
			Ambient_Colour[2] = 0.0f;
			Ambient_Colour[3] = 0.0f;
			break;
		case LightType::Ambient_Only:       //ambient only has no diffuse colour
			Direction[3] = 0.0f;
			Diffuse_Colour[0] = 0.0f;
			Diffuse_Colour[1] = 0.0f;
			Diffuse_Colour[2] = 0.0f;
			Diffuse_Colour[3] = 0.0f;
			break;
		case LightType::Spot:               //spotlight has a position and direction (initially at 1,1,1)
			Direction[3] = 1.0f;
			Spot_Direction[3] = 1.0f;
			break;
		default:                            //catchin any undefined behaviour
			Direction[3] = 0.0f;
			break;
	}
	
	toggleLight();                //enables the light
	Direction[0] = posDir.x;      //parses the position to the array
	Direction[1] = posDir.y;
	Direction[2] = posDir.z;

	//creates the source debug cube if enabled and the type supports it
	if (visibleSource && (type != LightType::Ambient_Only && type!= LightType::Directional_Only && type!= LightType::Ambient_Directional)) {
		debugCube = new Cube({0,0,0}, 0.2f);
	}
	glEnable(GL_LIGHTING);     //Enables lights in OpenGL
}

//calls appropriate OpenGl functions to apply light
void Light::applyLight()
{
	glPushMatrix();

	//applies transformations
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);
	
	assert(lightID < 0x4000 + 7);                                  //checks whether still in bounds
	
	//calls the OpenGL fucntions
		glLightfv(lightID, GL_POSITION, Direction);              
		glLightfv(lightID, GL_DIFFUSE, Diffuse_Colour);
		glLightfv(lightID, GL_AMBIENT, Ambient_Colour);
		if (type == LightType::Spot) {                             //spotlight requires some other calls as well
			glLightf(lightID, GL_SPOT_CUTOFF, spotCutoff);
			glLightfv(lightID, GL_SPOT_DIRECTION, Spot_Direction);
			glLightf(lightID, GL_SPOT_EXPONENT, spotExponent);
		}

	//renders the debug cube if availabel
	if (debugCube != nullptr) {
		glPushMatrix();
		glTranslatef(Direction[0],Direction[1],Direction[2]);     //lights position is Global but cube needs to be transformed
		toggleLight();
		debugCube->renderShape();
		toggleLight();
		glPopMatrix();
	}
	glPopMatrix();
	glPopMatrix();
}

//toggles the light, if on turns it off and vice-versa
void Light::toggleLight()
{
	assert (lightID < 0x4000 + 7);
	if (isOn) {
		glDisable(lightID);
	}
	else {
		glEnable(lightID);
	}
	isOn = !isOn;
}

//changes the position of the light by given offset
void Light::moveLight(Vertex offset)
{
	if (type == LightType::Ambient_Point || type == LightType::Point_Only || type == LightType::Spot) {  //available only for light types that have position
		Direction[0] += offset.x;
		Direction[1] += offset.y;
		Direction[2] += offset.z;
		hasMoved = true;                     //enables the flag for shadow recalculations
	}
}

// around the origin of transformation matrix before light render
void Light::orbitRotateLight(Rotator rotation)
{
	if (type != LightType::Ambient_Only) {
		this->rotation += rotation;
	}
}

// around the origin of transformation matrix before light render
void Light::orbitSetAngleLight(Rotator rotation)
{
	if (type != LightType::Ambient_Only) {
		this->rotation = rotation;
	}
}

//sets the diffuse colour
void Light::setDiffuseColour(Colour colour)
{
	if (type != LightType::Ambient_Only) {    //does not apply to ambient light only
		Diffuse_Colour[0] = colour.red;
		Diffuse_Colour[1] = colour.green;
		Diffuse_Colour[2] = colour.blue;
	}
}

//sets the am bient colour
void Light::setAmbientColour(Colour colour)
{
	if (type != LightType::Directional_Only && type!= LightType::Point_Only) {  //does not apply for directional lights and lon epoints light
		Ambient_Colour[0] = colour.red;
		Ambient_Colour[1] = colour.green;
		Ambient_Colour[2] = colour.blue;
	}
}

//sets the spotlight attributes 
void Light::setSpotAtributes(float cutoff, float exponent, Vertex spotDirection)
{
	if (type == LightType::Spot) {                 //only applies if the type is spotlight
		spotCutoff = cutoff;
		spotExponent = exponent;
		Spot_Direction[0] = spotDirection.x;
		Spot_Direction[1] = spotDirection.y;
		Spot_Direction[2] = spotDirection.z;
	}
}

//sets the attenuation for eligible lights
void Light::setAttenuation(float constant, float linear, float quadratic)
{
	if (type == LightType::Ambient_Point ||
		type == LightType::Point_Only ||
		type == LightType::Spot) 
	{
		glLightf(lightID, GL_CONSTANT_ATTENUATION, constant);
		glLightf(lightID, GL_LINEAR_ATTENUATION, linear);
		glLightf(lightID, GL_QUADRATIC_ATTENUATION, quadratic);
	}
}

// returns light position, differs per type
Vertex Light::getPosition()
{
	Vertex out = {0,0,0};
	if (type == LightType::Directional_Only || type == LightType::Ambient_Directional) {   //directional lights have an arbitrary position
		out.x = -Direction[0] * 200;
		out.y = -Direction[1] * 200;
		out.z = -Direction[2] * 200;
		return out;
	}
	else if (type == LightType::Ambient_Only) {    //Ambient lights have no valid position
		return out;
	}
	out.x = Direction[0];               //every other light uses its direction as position
	out.y = Direction[1];
	out.z = Direction[2];
	return out;
}

// gets the direction of the light, uses the object position or lights that dont have clear position
Vertex Light::getDirection(Vertex objectPos)
{
	Vertex out = {0,0,0};
	
	if (type == LightType::Ambient_Only) {   //ambient has no valid direction
		return out;
	}
	else if (type == LightType::Spot) {      //spotlights have clear direction defined
		out.x = Spot_Direction[0];
		out.y = Spot_Direction[1];
		out.z = Spot_Direction[2];
		return out;
	}
	else if (type == LightType::Directional_Only || type == LightType::Ambient_Directional) { //directional lights also have defined direction
		out.x = Direction[0];
		out.y = Direction[1];
		out.z = Direction[2];
		return out;
	}
	out.x = objectPos.x - Direction[0];      //every other type emits in all directions hence the vector to the object is used as position
	out.y = objectPos.y - Direction[1];
	out.z = objectPos.z - Direction[2];
	return out;
}
