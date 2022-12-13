// Below ifdef required to remove warnings for unsafe version of fopen and fscanf.
// Secure version won't work cross-platform, forcing this small hack.
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "model.h"

bool Model::load(char* modelFilename, char* textureFilename)
{
	bool result;

	// Load in the model data,
	result = loadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	loadTexture(textureFilename);

	return true;
}

bool Model::load(char* modelFilename)
{
	bool result;

	// Load in the model data,
	result = loadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

//loads the texture, flips the y UV coordinates of the vertices, used for ptoper importation of blender models
bool Model::load(char* modelFilename, bool TXCOORD_flipYaxis)
{
	bool out = loadModel(modelFilename);
	for (int i = 1; i < texCoord.size(); i += 2) {
		texCoord[i] = ((texCoord[i] - 0.5) * -1) + 0.5;             //mathematical algorithm to flip Y coordinates of the UVs
	}
	return false;
}

// creates a copy dynamically
Model* Model::copySelf()
{
	Model* out = new Model({0,0,0}, 1);    //creates a dummy dynamically (on the heap) 
	*out = *this;                          //copies the values of this model into the dummy
	return out;                            //return the pointer
}

//renders models as tris
void Model::render()
{
	ArrayRenderer::renderAllOrdered(GL_TRIANGLES, positions, normals, texCoord, colours);
}


// Modified from a mulit-threaded version by Mark Ropper.
bool Model::loadModel(char* filename)
{
	vector<Vector3> verts;
	vector<Vector3> norms;
	vector<Vector3> texCs;
	vector<unsigned int> faces;
	
	FILE* file = fopen(filename, "r");
	if (file == NULL)
	{
		return false;
	}
	while (true)
	{
		char lineHeader[128];

		// Read first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			break; // exit loop
		}
		else // Parse
		{
			if (strcmp(lineHeader, "v") == 0) // Vertex
			{
				Vector3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verts.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) // Tex Coord
			{
				Vector3 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				texCs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) // Normal
			{
				Vector3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				norms.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) // Face
			{
				unsigned int face[9];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &face[0], &face[1], &face[2],
																			&face[3], &face[4], &face[5],
																			&face[6], &face[7], &face[8]);
				if (matches != 9)
				{
					// Parser error, or not triangle faces
					return false;
				}
				
				for (int i = 0; i < 9; i++)
				{
					faces.push_back(face[i]);
				}


			}
		}
	}

#pragma region unroll_data
	// "Unroll" the loaded obj information into a list of triangles.

	for (int i = 0; i < faces.size(); i += 3) {              //loops though all the arrays and converts them to the proper vertex array system
		positions.push_back(verts[faces[i] - 1].x);
		positions.push_back(verts[faces[i] - 1].y);
		positions.push_back(verts[faces[i] - 1].z);

		texCoord.push_back(texCs[faces[i+1] - 1].x);
		texCoord.push_back(texCs[faces[i+1] - 1].y);

		normals.push_back(norms[faces[i+2] - 1].x);
		normals.push_back(norms[faces[i+2] - 1].y);
		normals.push_back(norms[faces[i+2] - 1].z);

		colours.push_back(1.0f);
		colours.push_back(1.0f);
		colours.push_back(1.0f);
		colours.push_back(1.0f);
	}


#pragma endregion unroll_data
		
	// Once data has been sorted clear read data (which has been copied and are not longer needed).
	verts.clear();
	norms.clear();
	texCs.clear();
	faces.clear();

	return true;
}

//uses my format for textures to create textures
void Model::loadTexture(char* filename)
{
	TEX_wrap_type wrapping[2] = { TEX_wrap_type::Repeat,TEX_wrap_type::Repeat };
	texture = new Texture(TEX_application_type::Modulate, wrapping, TEX_filtering_type::Trilinear_Point_MM_Bi, filename);
}



