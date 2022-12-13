#pragma once

#include "glut.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <vector>
#include "Vector3.h"
#include "Structs.h"

//handles some shadow funcitonality
class Shadow
{
public:
	static void generateShadowMatrix(Matrix4* shadowMatrix, Vertex light_pos, Vertex floorNormal, Vertex floorPoint);    //generates the perspective transformation matrix  
	static void extendVertex(Vertex& newVert, Vertex lightPosit, float x, float y, float z, float ext);                  //offsets a vertex in a direction of the line from source to vertex
	static std::vector<float> buildShadowVolume(Vertex lightPosit, std::vector<GLfloat> verts);                          //builds and returns a shadow volume 
};