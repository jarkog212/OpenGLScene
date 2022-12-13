
#include "Camera.h"

//constructor
Camera::Camera(Vertex position, Rotator rotation, int ID,int* screen_width, int* screen_height)
{
	this->position.setX(position.x);
	this->position.setY(position.y);
	this->position.setZ(position.z);

	this->rotation = rotation;
	this->ID = ID;

	this->screen_width = screen_width;
	this->screen_height = screen_height;

	glutSetCursor(GLUT_CURSOR_NONE);      //disables cursor visibility
}

//allows for a chnage og frustum settings
void Camera::redoFrustum(float new_Top, float new_Bottom, float new_Right, float new_Left, float new_Near, float new_Far)
{
	//save the settings of the frustum
	frustum.top = new_Top;
	frustum.bottom = new_Bottom;
	frustum.left = new_Left;
	frustum.right = new_Right;
	frustum.Fnear = new_Near;
	frustum.Ffar = new_Far;
	
	//switch the current actie frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(frustum.left, frustum.right, frustum.bottom, frustum.top, frustum.Fnear, frustum.Ffar);
	glMatrixMode(GL_MODELVIEW);
}

//calculates all the vectors of the camera object
void Camera::calculateCamera()
{
	//sin and cosine values of the camera directions/rotations (Roll,Pitch,Yaw)
	float sinY = sin(rotation.yaw() * TO_RADIANS);
	float cosY = cos(rotation.yaw() * TO_RADIANS);
	float sinP = sin(rotation.pitch() * TO_RADIANS);
	float cosP = cos(rotation.pitch() * TO_RADIANS);
	float sinR = sin(rotation.roll() * TO_RADIANS);
	float cosR = cos(rotation.roll() * TO_RADIANS);

	//mathematical formulas creating the forward and up vector from the angles
	RelativeForward.setX(sinY * cosP );
	RelativeForward.setY(sinP);
	RelativeForward.setZ(-cosY * cosP);

	RelativeUp.setX(-cosY * sinR - sinY * sinP * cosR);
	RelativeUp.setY(cosP * cosR);
	RelativeUp.setZ(-sinY * sinR - sinP * cosR * -cosY);

	//right vector is created as a cross product of up and forward vetors
	RelativeRight = RelativeForward.cross(RelativeUp);
}

//calls the OpenGL functions to apply camera with all the settings 
void Camera::applyCamera()
{
	Vector3 front = position + RelativeForward;         //creates an abstract point that is along the forward vector of the camera
	
	//OpenGL function applying the camera, called every frame
	gluLookAt(position.getX(),               //position of the camera
			  position.getY(),               //...
			  position.getZ(),               //...
			  front.getX(),                  //arbitrary point to look at 
			  front.getY(),                  //...
			  front.getZ(),                  //...
			  RelativeUp.getX(),             //up direction
			  RelativeUp.getY(),             //...
			  RelativeUp.getZ());            //...

	//if skybox is setup, repositions and renders the skybox
	if (skybox != nullptr) {
		skybox->repositionSkybox({ position.x, position.y, position.z });
		skybox->render();
	}
}

//encapsulates function into a single call
void Camera::apply()
{
	calculateCamera();
	applyCamera();
}

//adds to the position vector in the given direcion using the defined direction vector
void Camera::moveForward(float speed)
{
	Vector3 tempForward = RelativeForward;
	tempForward.scale(speed);               //scale is used to snap the positions closer or further from eachother, creating illusion of speed
	position += tempForward;
}

//adds to the position vector in the given direcion using the defined direction vector
void Camera::moveRight(float speed)
{
	Vector3 tempRight = RelativeRight;     
	tempRight.scale(speed);                 //scale is used to snap the positions closer or further from eachother, creating illusion of speed
	position += tempRight;
}

//adds to the position vector in the given direcion using the defined direction vector
void Camera::moveUp(float speed)
{
	Vector3 tempUp = RelativeUp;
	tempUp.scale(speed);                    //scale is used to snap the positions closer or further from eachother, creating illusion of speed
	position += tempUp;
}

//uses the mouseposition to determine by how much to rotate the camera
void Camera::mouseControl()
{
	int DX = input->getMouseX() - (*screen_width) / 2;            //gets the mouse offset from center of the screen (window) (x-axis)
	int DY = input->getMouseY() - (*screen_height) / 2;           //gets the mouse offset from center of the screen (window) (y-axis)  

	Rotator newRotation = {0,0,0};
	newRotation.yaw() = DX * rotation_sensitivity;                //uses the rotation sensitivity to find proper angle using the mouse offset 
	newRotation.pitch() = -DY * rotation_sensitivity;             //uses the rotation sensitivity to find proper angle using the mouse offset 
	rotateCamera(newRotation);                                    //adds the rotatuions to the current rotations
	
	glutWarpPointer((*screen_width) / 2, (*screen_height) / 2);   //resets the mouse position to the center of the screen (window)
}

//simply adds rotations to the current ones
void Camera::rotateCamera(Rotator rotation)
{
	this->rotation += rotation;
}

//all input handled by this class type 
void Camera::handleInput(float dt)
{
	if (input != nullptr) {
		if (input->isKeyDown('w')) {                //W for forward
			moveForward(cameraSpeed * dt);
			input->setKeyUp('w');
		}
		if (input->isKeyDown('s')) {                //S for backward (inverse forward)
			moveForward(-cameraSpeed * dt);
			input->setKeyUp('s');
		}
		if (input->isKeyDown('d')) {                //D for strafe to the right
			moveRight(cameraSpeed * dt);
			input->setKeyUp('d');
		}
		if (input->isKeyDown('a')) {                //A for strafe to the left (inverse right)
			moveRight(-cameraSpeed * dt);
			input->setKeyUp('a');
		}
		if (input->isKeyDown('e')) {                //E for mouse dissassociation
			active_controls = !active_controls;     //toggles flag
			glutSetCursor(GLUT_CURSOR_LEFT_SIDE);   //gets the right cursor icon and makes the cursor visible
			input->setKeyUp('e');
		}
		if (input->isKeyDown('x')) {                //X for upward movement
			moveUp(cameraSpeed * dt);
			input->setKeyUp('x');
		}
		if (input->isKeyDown('z')) {                //Z for downward movement (inverse upward)
			moveUp(-cameraSpeed * dt);
			input->setKeyUp('z');
		}
		if (input->isKeyDown('c')) {                //C for set rotation to the left
			Rotator newRot = { 0,0,0 };
			newRot.yaw() = -20 * dt;                //rotation is 20 degrees per second
			rotateCamera(newRot);
			input->setKeyUp('c');
		}
		if (input->isKeyDown('v')) {                //V for set rotation to right
			Rotator newRot = { 0,0,0 };
			newRot.yaw() = 20 * dt;                 //rotation is 20 degrees per second
			rotateCamera(newRot);
			input->setKeyUp('v');
		}
		if (active_controls) {                      //disables the mouse icon if needed to
			glutSetCursor(GLUT_CURSOR_NONE);
			mouseControl();
		}
	}
}

//creates anew skybox instance and sets it up
void Camera::setupSkybox(const char* textureAddress)
{
	//memory leak prevention
	if (skybox != nullptr) {
		delete skybox;   
	}
	skybox = new Skybox( textureAddress, { position.x, position.y, position.z }); //creates a new skybox and a new texture on heap
}

void Camera::setupSkybox(Texture* texture)
{
	//memory leak prevetion
	if (skybox != nullptr) {
		delete skybox;
	}
	skybox = new Skybox(texture, { position.x, position.y, position.z }); //reates a new skybox but uses already loaded texture
}
