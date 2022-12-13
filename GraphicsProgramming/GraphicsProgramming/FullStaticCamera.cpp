#include "FullStaticCamera.h"

//simple controls for fully static camera
void FullStaticCamera::handleInput(float dt)
{
	if (input != nullptr) {                          //prevents the nullptr exception             
		if (input->isKeyDown('e')) {                 // if E is pressed it disassociates the mouse 
			active_controls = !active_controls;      //toggles flag
			glutSetCursor(GLUT_CURSOR_LEFT_SIDE);    //shows cursor
			input->setKeyUp('e');
		}
	}
}
