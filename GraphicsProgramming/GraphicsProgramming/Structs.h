#pragma once
#define PI 3.1415
#define TO_RADIANS PI/180
#define TO_DEGREES 1/TO_RADIANS
#include "SOIL.h"
#include "glut.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cstdlib>
#include <string>
#include <vector>
#include "Vector3.h"
#include <cmath>;
#include <memory>
#include <unordered_map>

using namespace std;

//struct holding the colour variables (RGBA)
struct Colour {
	float red = 1.0f;
	float green = 1.0f;
	float blue = 1.0f;
	float alpha = 1.0f;

	Colour() {};
	Colour(float red, float green, float blue, float alpha) { this->red = red; this->green = green; this->blue = blue; this->alpha = alpha; }
	//multiplier by scalar, similar to vector multiplication by scalar, used for intensity
	Colour operator*(float scalar) {
		Colour out = { this->red * scalar, this->green * scalar, this->blue * scalar, 1.0f };
		return out;
	}
	//uses multiplier and saves the result
	void operator*=(float scalar) {
		Colour out = { this->red * scalar, this->green * scalar, this->blue * scalar, 1.0f };
		*this = out;
	}
	//calls the OpenGL function to assign a vertex colour to the "brush", adds alpha channel as well
	void applyColour() {
		glColor4f(red,green,blue,alpha);
	}
	//calls the OpenGL function to assign a vertex colour to the "brush"
	void applyColourNoAlpha() {
		glColor3f(red,green,blue);
	}
	//takes in the vertex array for colours pointer and adds the colour into it
	void pushColourToArray(vector<GLfloat>& colours) {
		colours.push_back(red);
		colours.push_back(green);
		colours.push_back(blue);
		colours.push_back(alpha);
	}
};

//struct that holds values for a single vertex
struct Vertex {
	float x = 0;         //position
	float y = 0;         //--
	float z = 0;         //--
	float nx = 0.0f;     //normals
	float ny = 1.0f;     //--
	float nz = 0.0f;     //--
	float UV_u = 0.0f;   //UVs
	float UV_v = 0.0f;   //--
	
	//sets the values for positions
	void setPosition(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	//sets the values for normals
	void setNormal(float x, float y, float z) {
		nx = x;
		ny = y;
		nz = z;
	}
	//sets the UVs
	void setTexCoord(float u, float v) {
		UV_u = u;
		UV_v = v;
	}
	//calls appropriate OpenGL functions to apply the vertex
	void applyVertexFull() {
		glTexCoord2f(UV_u, UV_v);
		glNormal3f(nx, ny, nz);
		glVertex3f(x, y, z);
	}
	//same as standard apply just without normals
	void applyVertexNoNormal() {
		glTexCoord2f(UV_u, UV_v);
		glVertex3f(x, y, z);
	}
	//only applies the position of the vertex
	void applyVertexSimple() {
		glVertex3f(x, y, z);
	}
	//takes in the pointers to the vertex arrays and adds the values apropriately
	void pushVertexToArray(vector<GLfloat> &positions, vector<GLfloat> &normals, vector<GLfloat> &texCoords) {
		positions.push_back(x);
		positions.push_back(y);
		positions.push_back(z);
		normals.push_back(nx);
		normals.push_back(ny);
		normals.push_back(nz);
		texCoords.push_back(UV_u);
		texCoords.push_back(UV_v);
	}
	//simple vector addition of positions 
	Vertex operator+(Vertex B) {
		Vertex out;
		out.x = this->x + B.x;
		out.y = this->y + B.y;
		out.z = this->z + B.z;
		return out;
	}
	//addtion with assign
	void operator+=(Vertex B) {
		*this = *this + B;
	}
	//simple scalar multiplication of a vector
	Vertex operator*(float multi) {
		Vertex out;
		out.x = this->x * multi;
		out.y = this->y * multi;
		out.z = this->z * multi;
		return out;
	}
	//multiplication with assign
	void operator*=(float multi) {
		*this = *this * multi;
	}
	//subtraction, uses inverted addition
	Vertex operator-(Vertex B) {
		B *= -1;
		return *this + B;
	}
	//subtraction with assign
	void operator-=(Vertex B) {
		*this = *this - B;
	}
	//normlizes the position vector
	void normalize() {
		float mag = magnitude();
		x /= mag;
		y /= mag;
		z /= mag;
	}
	//calculates the magnitude of the positions vector
	float magnitude() {
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}
};

//struct holding the rotation transforms
struct Rotator {
	float x = 0;       //rotation around x-axis (deg)
	float y = 0;       //rotation around y-axis (deg)
	float z = 0;       //rotation around z-axis (deg)
	//other model of 3D space + these should not be interchangeable with x,y,z as the R,P,Y. its just so the struct doesnt take more memory
	float& roll() { return z;}
	float& pitch() { return x; }
	float& yaw() { return y; }
	//adds rotator objects like vectors
	Rotator operator+(Rotator rot) {
		Rotator out = { this->x + rot.x , this->y + rot.y, this->z + rot.z };
		return out;
	}
	//addition with assign
	void operator+=(Rotator rot) {
		Rotator out = { this->x + rot.x , this->y + rot.y, this->z + rot.z };
		*this = out;
	}
	//multiplies rotations like vector multiplied by scalar 
	Rotator operator*(float multiplier) {
		Rotator out;
		out.x = x*multiplier;
		out.y = y*multiplier;
		out.z = z*multiplier;
		return out;
	}
	//multiplication with assign
	void operator*=(float multiplier) {
		Rotator out = *this*multiplier;
		*this = out;
	}
};

//predefined colours, primary and secondary colours, static function mean the object does not need to have a manually created instance
static struct DefinedColours {
	static Colour red() { return Colour(1.0f, 0.0f, 0.0f, 1.0f); };
	static Colour blue() { return Colour(0.0f, 0.0f, 1.0f, 1.0f); };
	static Colour green() { return Colour(0.0f, 1.0f, 0.0f, 1.0f); };
	static Colour cyan() { return Colour(0.0f, 1.0f, 1.0f, 1.0f); };
	static Colour magenta() { return Colour(1.0f, 0.0f, 1.0f, 1.0f); };
	static Colour yellow() { return Colour(1.0f, 1.0f, 0.0f, 1.0f); };
	static Colour white() { return Colour(1.0f, 1.0f, 1.0f, 1.0f); };
	static Colour black() { return Colour(0.0f, 0.0f, 0.0f, 0.0f); };
};

//enumerations holding specific keywords to be used as setting for textures
enum class TEX_application_type {Modulate, Decal};                                                                                            //Modulate blends, Decal acts like a sticker on top
enum class TEX_wrap_type {Repeat, Clamp};                                                                                                     //Repeat repeats the pattern, Clamp keeps the last values
enum class TEX_filtering_type {Point, Bilinear, Trilinear_Bi_MM_Point, Trilinear_Bi_MM_Bi, Trilinear_Point_MM_Point, Trilinear_Point_MM_Bi};  //Different types of filtering of textures

//Texture struct holding setting and applying the textures
struct Texture {
	
	double AppType = 0x2100;                      //holds application type - deprecated as it could have used GLenum
	double WrapType[2] = { 0x2900 , 0x2900 };     //holds wrapping for both x/y (s/t) for texture coords - deprecated as it could have used GLenum
	double Min_FiltType = 0x2600;                 //holds min filtering setting - deprecated as it could have used GLenum
	double Mag_FiltType = 0x2600;                 //holds mag filtering setting - deprecated as it could have used GLenum
	
	GLuint texture;                               //integer index for OpenGL texture array

	Texture() {};    //simple, bad constructor
	//constructor that is used, has to have all the texture settings specified, address is needed to load the texture form a file
	Texture(TEX_application_type AppType, TEX_wrap_type WrapType[2], TEX_filtering_type FiltType, const char* address) {   
		
		texture = SOIL_load_OGL_texture(                                               //OpenGL/SOIL loading of the images to memory and returning an index value of where in the memory texture array it is
			address,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
		
		if (texture == 0)                                                              //error handling
		{
			printf("SOIL loading error: '%s'\n", SOIL_last_result());
		}

		if (AppType == TEX_application_type::Decal) { this->AppType++; }                                                                //sets the application type in OpenGL terms, should use Glenum instead
		
		if (WrapType[0] == TEX_wrap_type::Repeat) { this->WrapType[0]++; };                                                             //sets the wrapping type in OpenGL terms, should use GLenum instead
		if (WrapType[1] == TEX_wrap_type::Repeat) { this->WrapType[1]++; };
		
		if (FiltType == TEX_filtering_type::Bilinear) { this->Mag_FiltType++; this->Min_FiltType++; }                                   //sets the filtering type in OpenGL terms, should use GLenum instead, uses switch to properly sort which values to use
		else if (FiltType != TEX_filtering_type::Point) {
			switch (FiltType) {
				case(TEX_filtering_type::Trilinear_Point_MM_Point): this->Mag_FiltType = 0x2600;  this->Min_FiltType = 0x2700; break;
				case(TEX_filtering_type::Trilinear_Bi_MM_Point): this->Mag_FiltType = 0x2601;  this->Min_FiltType = 0x2701; break;
				case(TEX_filtering_type::Trilinear_Point_MM_Bi): this->Mag_FiltType = 0x2600; this->Min_FiltType = 0x2702; break;
				default: this->Mag_FiltType = 0x2601; this->Min_FiltType = 0x2703; break;
			}
		}
	}
	//calls appropriate OpenGL functions to apply the texture, using the predefined settings
	void applyTexture() {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, AppType);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapType[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapType[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Mag_FiltType);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Min_FiltType);
	}
};

//simple Tri objects, used for debugging or if the tri needs to be drawn, not combinable with ProceduralShape
struct Tri {

	Vertex V1;      //first vertex
	Vertex V2;      //second vertex
	Vertex V3;      //third vertex
	Colour colour;  //colour for all vertices

	Tri(Vertex V1, Vertex V2, Vertex V3, Colour colour) { this->V1 = V1; this->V2 = V2; this->V3 = V3; this->colour = colour; };    //simple constructor to set up the tri, all three vertices and colour need to be defined
	//renders the tri using OpenGL function calls
	void drawTri()
	{
		colour.applyColour();
		glBegin(GL_TRIANGLES);
		V1.applyVertexSimple();
		V2.applyVertexSimple();
		V3.applyVertexSimple();
		glEnd();
	}
};

// static struct used to encapsulate all the different fucntion calls needed for rendering all arrays with OpenGL
static struct ArrayRenderer {
	//sets up the vertex arrays using the pointers to them, also tells OpenGL to enable vertex arrays
	static void setupVertexArrays(vector<GLfloat>& positions, vector<GLfloat>& normals, vector<GLfloat>& texCoords) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, positions.data());      //3 position values per vertex (x,z,y)
		glNormalPointer(GL_FLOAT, 0, normals.data());           //3 normal values per vertex (x,z,y)
		glTexCoordPointer(2,GL_FLOAT, 0, texCoords.data());     //2 UV values per vertex (u,v)
	}
	//overloaded setup function includes colour array
	static void setupVertexArrays(vector<GLfloat>& positions, vector<GLfloat>& normals, vector<GLfloat>& texCoords, vector<GLfloat>& colours) {	
		setupVertexArrays(positions,normals,texCoords);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, colours.data());         //4 colour values per vertex (R,G,B,A)
	}
	//cleanes up the OpenGL by disabling the arrays
	static void endVertexArrays() {
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}
	//renders all vertex arrays assuming they are all ordered in the render order, each entry is used only once
	static void renderAllOrdered(GLenum mode, vector<GLfloat>& positions, vector<GLfloat>& normals, vector<GLfloat>& texCoords) {
		setupVertexArrays(positions, normals, texCoords);
		glDrawArrays(mode,0, positions.size() / 3);
		endVertexArrays();
	}
	//overloaded function that renders all vertices assuming they are ordered, colours included
	static void renderAllOrdered(GLenum mode, vector<GLfloat>& positions, vector<GLfloat>& normals, vector<GLfloat>& texCoords, vector<GLfloat>& colours) {
		setupVertexArrays(positions, normals, texCoords, colours);
		glDrawArrays(mode, 0, positions.size() / 3);
		endVertexArrays();
	}
	// renders all vertices according to the indices array, elements can be used multiple times
	static void renderAllIndexedOrder(GLenum mode, vector<GLfloat>& positions, vector<GLfloat>& normals, vector<GLfloat>& texCoords, vector<GLuint>& order) {
		setupVertexArrays(positions, normals, texCoords);
		glDrawElements(mode, order.size() , GL_UNSIGNED_INT, order.data());
		endVertexArrays();
	}
	// renders all vertices according to the indices array with colour
	static void renderAllIndexedOrder(GLenum mode, vector<GLfloat>& positions, vector<GLfloat>& normals, vector<GLfloat>& texCoords, vector<GLfloat>& colours, vector<GLuint>& order) {
		setupVertexArrays(positions, normals, texCoords, colours);
		glDrawElements(mode, order.size(), GL_UNSIGNED_INT, order.data());
		endVertexArrays();
	}
};

//struct that is representig a 4x4 matrix, used for matrix calculations
struct Matrix4 {
	float matrix[4][4] = {                   //2D array acting like a matrix
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}};
	
	//multiplies the matrices according to the matrix multiplication 
	Matrix4 operator*(Matrix4 B) {
		Matrix4 out;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				out.matrix[i][j] = 0;
				for (int m = 0; m < 4; m++) {
					out.matrix[i][j] += this->matrix[i][m] * B.matrix[m][j];
				}
			}
		}
		return out;
	}
	//matrix addition, adding elements one by one
	Matrix4 operator+(Matrix4 B) {
		Matrix4 out;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				out.matrix[i][j] = this->matrix[i][j] + B.matrix[i][j];
			}
		}
		return out;
	}
	//matrix multiplication with assign
	void operator*=(Matrix4 B) {
		*this = *this * B;
	}
	//matrix addition with assign
	void operator+=(Matrix4 B) {
		*this = *this + B;
	}
	//consider the first 3 columns of a row a vector and divides it by the 4 column in the same row
	void normalizeRow(int i) {
		if (matrix[i][3] != 0) {
			matrix[i][0] /= matrix[i][3];
			matrix[i][1] /= matrix[i][3];
			matrix[i][2] /= matrix[i][3];
		}
	}
    //essentially creates an identity matrix, matches the world axes vectors, used for transitions between global and local scope
	void makeWorldAxes() {
		float out[4][4] = {
			{1,0,0,1},
			{0,1,0,1},
			{0,0,1,1},
			{0,0,0,1}
		};
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				matrix[i][j] = out[i][j];
			}
		}
	}
	//builds the rotation matrix using the OpenGL formula which rotates around a vector 
	void buildRotationMatrix(float angle, float rotVx, float rotVy, float rotVz) {
		float c = cos(angle * TO_RADIANS);
		float s = sin(angle * TO_RADIANS);
		float out[4][4] = {
			{pow(rotVx,2)*(1-c)+c , rotVx*rotVy*(1-c)-rotVz*s , rotVx*rotVz*(1-c)+rotVy*s , 0},
			{rotVx*rotVy*(1-c)+rotVz*s , pow(rotVy,2)*(1-c)+c , rotVy*rotVz*(1-c)-rotVx*s , 0},
			{rotVx*rotVz*(1-c)-rotVy*s , rotVy*rotVz*(1-c)+rotVx*s , pow(rotVz,2)*(1-c)+c , 0},
			{0 , 0 , 0 , 1}
		};
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				matrix[i][j] = out[i][j];
			}
		}
	}
};
