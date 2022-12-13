#include "StaticCamera.h"

//more limited handle input with only fe key bindings
void StaticCamera::handleInput(float dt)
{
	if (input != nullptr) {
		if (input->isKeyDown('e')) {                //E dissassociates the mouse form camera
			active_controls = !active_controls;
			glutSetCursor(GLUT_CURSOR_LEFT_SIDE);   //displays the cursor back
			input->setKeyUp('e');
		}
		if (input->isKeyDown('c')) {                //C rotates by a set angle to the left     
			Rotator newRot = { 0,0,0 };
			newRot.yaw() = -20 * dt;
			rotateCamera(newRot);
			input->setKeyUp('c');
		}
		if (input->isKeyDown('v')) {                //V rotates by a set angle to the right
			Rotator newRot = { 0,0,0 };
			newRot.yaw() = 20 * dt;
			rotateCamera(newRot);
			input->setKeyUp('v');
		}
		if (active_controls) {                      //uses mouse offset from center of the window to move camera
			glutSetCursor(GLUT_CURSOR_NONE);        //disables cursor
			mouseControl();
		}
	}
}
