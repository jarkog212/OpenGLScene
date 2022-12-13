#include "Billboard.h"
#include "Camera.h"

//matches the rotation of the plane with the camera
void Billboard::rotateToCamera()
{
	Vertex A = { (*activeCamera)->getPosition().x - center.x,(*activeCamera)->getPosition().y - center.y,(*activeCamera)->getPosition().z - center.z }; //get vector from plane center to camera
	A.normalize();                                                                                                                                      //make it a unit vector
	Vertex B = getWorldNormal();                                                                                                                        //get the curren normal of the plane
	B.y = 0;                                                                                                                                            //ignore y as were not changing the pitch
	A.y = 0;                                                                                                                                            //...
	float angle = atan2(B.z, B.x) - atan2(A.z, A.x);                                                                                                    //find the angle between the 2 vectors (rad)
	angle *= TO_DEGREES;                                                                                                                                //change it to derees
	rotation.y += angle;                                                                                                                                //add it to the current yaw (y) rotation
}

Billboard::~Billboard()
{
}

//creates a dynamic (heap) copy of self
Billboard* Billboard::copySelf()
{
	Billboard* out = new Billboard({0,0,0}, 1);                    //create a dummy dynamically
	*out = *this;                                                  //overwrite the dummy with its own copy
	return out;                                                    //return pointer to it
}

//update needs to call rotation adjustement
void Billboard::update(float dt)
{
	rotateToCamera();                                       
}
