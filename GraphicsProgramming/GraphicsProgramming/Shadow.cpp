#include "Shadow.h"

// For planar shadows. Provide a float shadowmatrix[16] to fill, the light position and the coordinates the describe the floor/geometry to cast the shadow on.
void Shadow::generateShadowMatrix(Matrix4* shadowMatrix, Vertex light_pos, Vertex floorNormal, Vertex floorPoint) {

	//Equation of plane is ax + by + cz = d
	//a, b and c are the coefficients of the normal to the plane (i.e. normal = ai + bj + ck)
	//If (x0, y0, z0) is any point on the plane, d = a*x0 + b*y0 + c*z0
	//i.e. d is the dot product of any point on the plane (using P here) and the normal to the plane
	float a, b, c, d;
	a = floorNormal.x;
	b = floorNormal.y;
	c = floorNormal.z;
	d = floorNormal.x*floorPoint.x + floorNormal.y*floorPoint.y + floorNormal.z*floorPoint.z;
	//Origin of projection is at x, y, z. Projection here originating from the light source's position
	float x, y, z;
	x = light_pos.x;
	y = light_pos.y;
	z = light_pos.z;

	//This is the general perspective transformation matrix from a point (x, y, z) onto the plane ax + by + cz = d
	(*shadowMatrix).matrix[0][0] = d - (b * y + c * z);
	(*shadowMatrix).matrix[0][1] = a * y;
	(*shadowMatrix).matrix[0][2] = a * z;
	(*shadowMatrix).matrix[0][3] = a;

	(*shadowMatrix).matrix[1][0] = b * x;
	(*shadowMatrix).matrix[1][1] = d - (a * x + c * z);
	(*shadowMatrix).matrix[1][2] = b * z;
	(*shadowMatrix).matrix[1][3] = b;

	(*shadowMatrix).matrix[2][0] = c * x;
	(*shadowMatrix).matrix[2][1] = c * y;
	(*shadowMatrix).matrix[2][2] = d - (a * x + b * y);
	(*shadowMatrix).matrix[2][3] = c;

	(*shadowMatrix).matrix[3][0] = -d * x;
	(*shadowMatrix).matrix[3][1] = -d * y;
	(*shadowMatrix).matrix[3][2] = -d * z;
	(*shadowMatrix).matrix[3][3] = -(a * x + b * y + c * z);
}



// Builds and returns the shadow volume. Provide the light position and a float vector of the vertices of the shape casting the shadow.
// Will extend caster vertices to create shadow volume. Shadow volume is returned as a vertex array/vector for easy rendering.
std::vector<float> Shadow::buildShadowVolume(Vertex lightPosit, std::vector<GLfloat> verts)
{
	std::vector<float> shadowVolume;
	float extrusion = 5.f;

	// Clear previous shadow volume
	shadowVolume.clear();

	//Build new shadow volume

	// Temporary variable for storing newly calculated vertcies
	Vertex vExtended;


	// For each vertex of the shadow casting object, find the edge 
	// that it helps define and extrude a quad out from that edge.
	for (int i = 0; i < verts.size(); i += 3)
	{
		// Define the edge we're currently working on extruding...
		int e0 = i;
		int e1 = i + 3;

		// If the edge's second vertex is out of array range, 
		// place it back at 0
		if (e1 >= verts.size())
		{
			e1 = 0;
		}
		// v0 of our extruded quad will simply use the edge's first 
		// vertex or e0.
		shadowVolume.push_back(verts[e0]);
		shadowVolume.push_back(verts[e0 + 1]);
		shadowVolume.push_back(verts[e0 + 2]);

		// v1 of our quad is created by taking the edge's first 
		// vertex and extending it out by some amount.
		extendVertex(vExtended, lightPosit, verts[e0], verts[e0 + 1], verts[e0 + 2], extrusion);
		shadowVolume.push_back(vExtended.x);
		shadowVolume.push_back(vExtended.y);
		shadowVolume.push_back(vExtended.z);

		// v2 of our quad is created by taking the edge's second 
		// vertex and extending it out by some amount.
		extendVertex(vExtended, lightPosit, verts[e1], verts[e1 + 1], verts[e1 + 2], extrusion);
		shadowVolume.push_back(vExtended.x);
		shadowVolume.push_back(vExtended.y);
		shadowVolume.push_back(vExtended.z);

		// v3 of our extruded quad will simply use the edge's second 
		//// vertex or e1.
		shadowVolume.push_back(verts[e1]);
		shadowVolume.push_back(verts[e1 + 1]);
		shadowVolume.push_back(verts[e1 + 2]);
	}
	return shadowVolume;
}

// Part of the shadow volume calculation. Calculates an extended vertex value based on light position, original vertex and extrusion/extend value.
void Shadow::extendVertex(Vertex& newVert, Vertex lightPosit, float x, float y, float z, float ext)
{
	Vertex lightDir;

	// Create a vector that points from the light's position to the original vertex.
	lightDir.x = -(- x + lightPosit.x);
	lightDir.y = -(- y + lightPosit.y);
	lightDir.z = -(- z + lightPosit.z);

	// Then use that vector to extend the original vertex out to a new position.
	// The distance to extend or extrude the new vector is specified by t.
	newVert.x = lightPosit.x + lightDir.x * ext;
	newVert.y = lightPosit.y + lightDir.y * ext;
	newVert.z = lightPosit.z + lightDir.z * ext;
}

