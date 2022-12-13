// Scene class. Configures a basic 3D scene.
// Interfaces with the Input class to handle user input
// Calculates and outputs Frames Per Second (FPS) rendered.
// Important functions are the constructor (initialising the scene), 
// update (for process user input and updating scene objects) and render (renders scene).
#ifndef _SCENE_H
#define _SCENE_H
#pragma once
// Include GLUT, openGL, input.
/*#include "glut.h"
#include <gl/GL.h>
#include <gl/GLU.h>*/
#include "Input.h"
#include <stdio.h>
// Further includes should go here:
#include "SOIL.h"
#include <vector>
#include "Shadow.h"
#include "CameraManager.h"
#include "FullStaticCamera.h"
#include "Light.h"
#include "Disc.h"
#include "Cylinder.h"
#include "Model.h"
#include "Sphere.h"
#include "Mirror.h"
#include "Billboard.h"
#include "DetailedCube.h"
#include "Library.h"

class Scene {

public:
	Scene(Input* in);
	// Main render function
	void render();
	// Handle input function that receives delta time from parent.
	void handleInput(float dt);
	// Update function receives delta time from parent (used for frame independent updating).
	void update(float dt);
	// Resizes the OpenGL output based on new window size.
	void resize(int w, int h);
	void renderShadowScene();

protected:
	// configure opengl render pipeline
	void initialiseOpenGL();
	// Renders text (x, y positions, RGB colour of text, string of text to be rendered)
	void displayText(float x, float y, float r, float g, float b, char* string);
	// A function to collate all text output in a single location
	void renderTextOutput();
	void calculateFPS();
	//functions that encapsulate different setup steps for objects, used for tidy code, no array variant doesn't add it to the geometry vector 
	void setupObject(ProceduralShape* object, Vertex scaling, Rotator rotation, std::string materialName, std::string textureName, bool globalRotation);
	void setupTransparentObject(ProceduralShape* object, Vertex scaling, Rotator rotation, std::string materialName, std::string textureName, bool globalRotation);
	void setupObjectNoArray(ProceduralShape* object, Vertex scaling, Rotator rotation, std::string materialName, std::string textureName, bool globalRotation);
	void setupClothPile(Model* pile, int num, DetailedCube* parent, Vertex relativePosition, Rotator rotation, Vertex scaling, std::string materialName);
	void setupClothPileNoShadow(Model* pile, int num, ProceduralShape* parent, Vertex relativePosition, Rotator rotation, Vertex scaling, std::string materialName);
	// objects
	void initialiseObjects();                    //initialises all the objects 
	CameraManager* cameras;
	Light light0 = Light(LightType::Ambient_Point, 0, { 0,4,0 }, false);     //point
	Light light1 = Light(LightType::Point_Only, 1, { 7.5,2.22,3 }, false);   //point
	Light light2 = Light(LightType::Point_Only, 2, { 7.5,2.22,-2 }, false);  //point
	Light light3 = Light(LightType::Point_Only, 3, { -7.5,2.22,3 }, false);  //point
	Light light4 = Light(LightType::Point_Only, 4, { -7.5,2.22,-2 }, false); //point
	Light light5 = Light(LightType::Spot, 5, { -4.65,4.7,-1.06 }, false);    //spot
	Light light6 = Light(LightType::Spot, 6, { 4.65,4.7,-1.06 }, false);     //spot
	Library library;                          //holds assets
	const float clothS = 0.617;               //constant scale factor for small cloth pile
	float elapsedTime = 0;                    //tracks the time in ms from the start
	bool wireFrameMode = false;               //flag for wireframe mode

	std::vector<std::shared_ptr<ProceduralShape>> geometry;             //holds all the nontransparent parent objects
	std::vector<std::shared_ptr<ProceduralShape>> transparentGeometry;  //hold the tyransparent objects

	std::vector<Light*> lights = { &light0,&light1,&light2,&light3,&light4,&light5,&light6 }; //holds lights

	//most of the objects in the scene are declared, some are done dynamically. Indentation signifies the parent children relationship

	//shadow dummy planes, used to either cast or receive shadows
	Plane D_SideTableLB = Plane(1, 1, 1.383 * 2, 0.617 * 2, { 4.95,1.13,5.55 });
	Plane D_SideTableLT = Plane(1, 1, 1.383 * 2, 0.617 * 2, { 4.95,4.9,5.55 });
	Plane D_SideTableRB = Plane(1, 1, 1.383 * 2, 0.617 * 2, { -4.95,1.13,5.68 });
	Plane D_SideTableRT = Plane(1, 1, 1.383 * 2, 0.617 * 2, { -4.95,4.9,5.68 });
	Plane D_FrontTable = Plane(1, 1, 1 * 2, 0.45 * 2, { 0,1.72,3.45 });

	//front wall
	DetailedCube frontWoodPanel = DetailedCube({ 0, 2.5, 6 },1,50);
	Plane backWall = Plane(100 ,100, 14, 5, { 0,2.5,6 });
	Plane blueBackgroundFL = Plane(1, 1, 4, 0.6, { 5.04,0.35,5.9973 });
	Plane blueBackgroundFR = Plane(1, 1, 4, 0.6, { -5.04,0.35,5.9973 });
	Cube logoPanelL = Cube({ 2.9,4,6 }, 1);
	Cube logoPanelR = Cube({ -2.9,4,6 }, 1);
	Model camineuSign = Model({ -0.02,3.3,5.85 }, 1);
	Model sideTableL = Model({ 3,0.06,5.5 }, 1);
		Model clothesSL1 = Model({ 1,1,1 }, 1);
		Model clothesSL2 = Model({ 1,1,1 }, 1);
		Model clothesSL3 = Model({ 1,1,1 }, 1);
		Model clothesSL4 = Model({ 1,1,1 }, 1);
		Model clothesSL5 = Model({ 1,1,1 }, 1);
		Model clothesSL6 = Model({ 1,1,1 }, 1);
		Model clothesSL7 = Model({ 1,1,1 }, 1);
		Model clothesSL8 = Model({ 1,1,1 }, 1);
	Cube LED_L = Cube({ 5.4,4.85,5.5 }, 1);
	Model sideTableR = Model({ -3,0.06,5.5 }, 1);
		Model clothesSR1 = Model({ 1,1,1 }, 1);
		Model clothesSR2 = Model({ 1,1,1 }, 1);
		Model clothesSR3 = Model({ 1,1,1 }, 1);
		Model clothesSR4 = Model({ 1,1,1 }, 1);
		Model clothesSR5 = Model({ 1,1,1 }, 1);
		Model clothesSR6 = Model({ 1,1,1 }, 1);
		Model clothesSR7 = Model({ 1,1,1 }, 1);
		Model clothesSR8 = Model({ 1,1,1 }, 1);
		Model clothesSR9 = Model({ 1,1,1 }, 1);
	Cube LED_R = Cube({ -3.8,4.85,5.5 }, 1);
	Model jacket = Model({ 5.15,3.176,5.64 }, 1);
	Model vest1 = Model({ -4.64,3.556,5.85 }, 1);
	Model vest2 = Model({ -6.07,2.8,5.86 }, 1);
	Model vest3 = Model({ -6.07,4.14,5.86 }, 1);
	Cylinder plantPotL = Cylinder({ 2.51,0.35,5.4 }, 0.381*2, 0.381*2, 5, 16);
	Cylinder plantPotR = Cylinder({ -2.51,0.35,5.4 }, 0.381 * 2, 0.381 * 2, 5, 16);

	//right wall
	DetailedCube pillarR1 = DetailedCube({ -7.5,2.5,5.5 }, 1,10);
	DetailedCube pillarR2 = DetailedCube({ -7.5,2.5,0.5 }, 1,10);
	DetailedCube pillarR3 = DetailedCube({ -7.5,2.5,-4.5 }, 1,10);
	DetailedCube pillarR4 = DetailedCube({ -7.5,2.5,-5.5 }, 1,10);
	DetailedCube overheadR1 = DetailedCube({ -7.5,4.5,3 }, 1,10);
	DetailedCube overheadR2 = DetailedCube({ -7.5,4.5,-2 }, 1,10);
	Cube seatR1 = Cube({ -7.5,0.5,3 }, 1);
	Cube seatR2 = Cube({ -7.5,0.5,-2 }, 1);
	Plane backWallR = Plane(50, 50, 12, 5, { -8,2.5,0 });
	Plane metalBarR1A = Plane(2, 2, 4.1, 0.08, { -7.5,3.268,3 });
	Plane metalBarR1B = Plane(2, 2, 4.1, 0.08, { -7.5,3.268,3 });
	Plane metalBarR2A = Plane(2, 2, 4.1, 0.08, { -7.5,3.268,-2 });
	Plane metalBarR2B = Plane(2, 2, 4.1, 0.08, { -7.5,3.268,-2 });
	Plane blueBackgroundSR = Plane(1, 1, 12, 0.6, { -6.997,0.35,0});
	Model brandSignR1 = Model({ -7.14,4.19,3 }, 1);
	Model brandSignR2 = Model({ -7.14,4.19,-2 }, 1);
	Plane TshirtsTR = Plane(2, 2, 0.6 * 2, 0.97 * 2, { -7.4,2.4,4.74 });
	Plane TshirtsBR = Plane(2, 2, 0.6 * 2, 0.97 * 2, { -7.4,2.4,-0.22 });

	//left wall
	DetailedCube pillarL1 = DetailedCube({ 7.5,2.5,5.5 }, 1,10);
	DetailedCube pillarL2 = DetailedCube({ 7.5,2.5,0.5 }, 1,10);
	DetailedCube pillarL3 = DetailedCube({ 7.5,2.5,-4.5 }, 1,10);
	DetailedCube pillarL4 = DetailedCube({ 7.5,2.5,-5.5 }, 1,10);
	DetailedCube overheadL1 = DetailedCube({ 7.5,4.5,3 }, 1,10);
	DetailedCube overheadL2 = DetailedCube({ 7.5,4.5,-2 }, 1,10);
	Cube seatL1 = Cube({ 7.5,0.5,3 }, 1);
	Cube seatL2 = Cube({ 7.5,0.5,-2 }, 1);
	Plane backWallL = Plane(50, 50, 12, 5, { 8,2.5,0 });
	Plane metalBarL1A = Plane(1, 1, 4.1, 0.08, { 7.5,3.268,3 });
	Plane metalBarL1B = Plane(1, 1, 4.1, 0.08, { 7.5,3.268,3 });
	Plane metalBarL2A = Plane(1, 1, 4.1, 0.08, { 7.5,3.268,-2 });
	Plane metalBarL2B = Plane(1, 1, 4.1, 0.08, { 7.5,3.268,-2 });
	Plane blueBackgroundSL = Plane(1, 1, 12, 0.6, { 6.997,0.35,0 });
	Model brandSignL1 = Model({ 7.14,4.19,3 }, 1);
	Model brandSignL2 = Model({ 7.14,4.19,-2 }, 1);
	Plane TshirtsTL = Plane(2, 2, 0.6 * 2, 0.97 * 2, { 7.4,2.4,4.74 });
	Plane TshirtsBL = Plane(2, 2, 0.6 * 2, 0.97 * 2, { 7.4,2.4,-0.22 });

	//middle
	DetailedCube tableTL = DetailedCube({ 3,0.68,0 },1,8);
		Model clothesTLB1 = Model({ 1,1,1 }, 1);
		Model clothesTLB2 = Model({ 1,1,1 }, 1);
		Model clothesTLB3 = Model({ 1,1,1 }, 1);
		Model clothesTLB4 = Model({ 1,1,1 }, 1);
		Model clothesTLT1 = Model({ 1,1,1 }, 1);
		Model clothesTLT2 = Model({ 1,1,1 }, 1);
		Model clothesTLT3 = Model({ 1,1,1 }, 1);
	Cube footTL = Cube({ 3,0.075,0 }, 1);
	DetailedCube tableTR = DetailedCube({ -3,0.68,0 }, 1,8);
		Model clothesTRB1 = Model({ 1,1,1 }, 1);
		Model clothesTRB2 = Model({ 1,1,1 }, 1);
		Model clothesTRB3 = Model({ 1,1,1 }, 1);
		Model clothesTRT1 = Model({ 1,1,1 }, 1);
		Model clothesTRT2 = Model({ 1,1,1 }, 1);
		Model clothesTRT3 = Model({ 1,1,1 }, 1);
	Cube footTR = Cube({ -3,0.075,0 }, 1);
	DetailedCube tableBL = DetailedCube({ 3,0.68,-3 }, 1,8);
		Model clothesBLB1 = Model({ 1,1,1 }, 1);
		Model clothesBLB2 = Model({ 1,1,1 }, 1);
		Model clothesBLB3 = Model({ 1,1,1 }, 1);
		Model clothesBLB4 = Model({ 1,1,1 }, 1);
		Model clothesBLT1 = Model({ 1,1,1 }, 1);
		Model clothesBLT2 = Model({ 1,1,1 }, 1);
	Cube footBL = Cube({ 3,0.075,-3 }, 1);
	DetailedCube tableBR = DetailedCube({ -3,0.68,-3 }, 1,8);
		Model clothesBRB1 = Model({ 1,1,1 }, 1);
		Model clothesBRB2 = Model({ 1,1,1 }, 1);
		Model clothesBRB3 = Model({ 1,1,1 }, 1);
		Model clothesBRB4 = Model({ 1,1,1 }, 1);
		Model clothesBRT1 = Model({ 1,1,1 }, 1);
		Model clothesBRT2 = Model({ 1,1,1 }, 1);
		Model clothesBRT3 = Model({ 1,1,1 }, 1);
		Model clothesBRT4 = Model({ 1,1,1 }, 1);
	Cube footBR = Cube({ -3,0.075,-3 }, 1);
	Model frontTable = Model({ 0,1.5,3.5 }, 1);
	Cylinder miniPlantPot = Cylinder({ -1.36,1.84,3.5 }, 0.151 * 2, 0.151 * 2, 5, 12);
	Model woodEdge = Model({ 0,0,0 }, 1);

	//transparent obj need to be last for the best blending
	Billboard plantL = Billboard({ 2.55,1.68,5.38 }, 0.368 * 2, 1 * 2);
	Billboard plantR = Billboard({ -2.5,1.68,5.38 }, 0.368 * 2, 1 * 2);
	Billboard miniPlant = Billboard({ -1.35,2.37,3.5 }, 0.145 * 2, 0.395 * 2);
	Plane monitorL = Plane(1, 1, 0.242 * 2, 0.242 * 2, { 1.15,2.06,3.4 });
		Plane monitorLS1 = Plane(1, 1, 0.105 * 2, 0.105 * 2, { -0.105,0.105,0 });
		Plane monitorLS2 = Plane(1, 1, 0.04 * 2, 0.04 * 2, { 0.105,0.105,-0.105 });
	Plane monitorR = Plane(1, 1, 0.242 * 2, 0.242 * 2, { 0.47,2.06,3.26 });
		Plane monitorRS1 = Plane(1, 1, 0.105 * 2, 0.105 * 2, { -0.105,0.105,-0.105 });
		Plane monitorRS2 = Plane(1, 1, 0.105 * 2, 0.105 * 2, { 0.105,0.105,0.105 });
	Plane keyboard = Plane(1, 1, 0.395 * 2, 0.145 * 2, { 0.8,1.75,3.7 });
	Plane signTLT = Plane(1, 1, 0.288 * 2, 0.345 * 2, { 3.36,1.9,0.13 });
	Plane signTLB = Plane(1, 1, 0.288*2, 0.345*2, { 3.36,1.9,-0.14 });
	Plane signBLT = Plane(1, 1, 0.288 * 2, 0.345 * 2, { 3.36,1.9,-2.84 });
	Plane signBLB = Plane(1, 1, 0.288 * 2, 0.345 * 2, { 3.36,1.9,-3.12 });
	Plane signTRT = Plane(1, 1, 0.288 * 2, 0.345 * 2, { -2.65,1.9,0.13 });
	Plane signTRB = Plane(1, 1, 0.288 * 2, 0.345 * 2, { -2.65,1.9,-0.14 });
	Plane signBRT = Plane(1, 1, 0.288 * 2, 0.345 * 2, { -2.65,1.9,-2.84 });
	Plane signBRB = Plane(1, 1, 0.288 * 2, 0.345 * 2, { -2.65,1.9,-3.12 });

	//reflective floor
	Mirror floor = Mirror({ 0,0,0 }, { 0,0,0 }, 14, 12, 120);

	// For access to user input.
	Input* input;
		
	// For Window and frustum calculation.
	int width, height;
	float fov, nearPlane, farPlane;

	// For FPS counter and mouse coordinate output.
	int frame = 0, time, timebase = 0;
	char fps[40];
	char mouseText[40];

};

#endif