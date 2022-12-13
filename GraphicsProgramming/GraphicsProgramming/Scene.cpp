#include "Scene.h"

// Scene constructor, initilises OpenGL
// You should add further variables to need initilised.
Scene::Scene(Input *in)
{
	// Store pointer for input class
	input = in;
	initialiseOpenGL();

	// Other OpenGL / render setting should be applied here.
	glEnable(GL_NORMALIZE);                            //makes sure normals do not get distorted by scaling
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Initialise scene variables#
	//cameras being initialised with their manager, stored in it for easier manipulation. all done on the heap
	cameras = new CameraManager(in);                                   
	cameras->addCamera(new Camera({ 0,1,0 }, { 0,180,0 }, 0, &width, &height));
	cameras->addCamera(new FullStaticCamera({ 0,2,-10 }, { -5,180,0 }, 1, &width, &height));
	cameras->addCamera(new StaticCamera({ 8.18,8.57,-13.94 }, { -25,-150,0 }, 2, &width, &height));
	cameras->addCamera(new StaticCamera({ -8.18,8.57,-13.94 }, { -25,150,0 }, 3, &width, &height));
	cameras->addCamera(new FullStaticCamera({ 0.368,17,-9 }, { -60,180,0 }, 4, &width, &height));
	//lights initialisation with types and colours
	light0.setAmbientColour(DefinedColours::white()*0.4);
	light0.setDiffuseColour(DefinedColours::white()*0.6);
	for (int i = 0; i < 4; i++) {
	lights[i+1]->setDiffuseColour({ 1,0.8,0.2,1 });
	lights[i+1]->setAttenuation(0.2, 0.05, 0.1);
	}
	for (int i = 0; i < 2; i++) {
		lights[i + 5]->setDiffuseColour({ 1,1,1,1 });
		lights[i + 5]->setSpotAtributes(45/2,35, { 0,-0.25,1 });
	}
	//all the objects are initialised
	initialiseObjects();
}

void Scene::handleInput(float dt)
{
	// Handle user input
	if (input->isMouseRDown()) {                          //if mouse right button is pressed...
		if (wireFrameMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //...either disable or...
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    //...enable wireframe mode
		}
		wireFrameMode = !wireFrameMode;                   //toggle the flag
		input->setMouseRDown(false);
	}
	if (input->isMouseLDown()) {                          //if mouse left button i spressed...
		for (auto it : lights) {                          //...cycle through lights...
			it->toggleLight();                            //...and toggle them
		}
		input->setMouseLDown(false);
	}
	cameras->handleInput(dt);                             //call camera mnager's handle input to then have it properly forwarded to the active camera
}

void Scene::update(float dt)
{
	// update scene related variables.
	elapsedTime += dt;                               //keeps track of elapsed time in ms, only increasing

	Vertex sinMov = { 0,0.001,0 };                   //max movement offset per one frame
	Vertex cosMov = { 0,0.001,0 };                   //max movement offset per one frame

	sinMov *= sin(elapsedTime);                      //multiply the max offset by sin of time, sin is oscilating between (-1) - 1 hence the movement will be oscilatory
	cosMov *= cos(elapsedTime);                      //multiply the max offset by cos of time (to give offset or delay form sine), cos is oscilating between (-1) - 1 hence the movement will be oscilatory

	//move the tables accordingly
	tableTL.setCenter(tableTL.getCenter() + sinMov);
	tableBR.setCenter(tableBR.getCenter() + sinMov);
	tableTR.setCenter(tableTR.getCenter() + cosMov);
	tableBL.setCenter(tableBL.getCenter() + cosMov);
	light0.moveLight(sinMov*10);                     //light also moves but only a bit 


	floor.update(dt);                                //update the floor, check whether new reflections need to be calculated (they do as the tables move)
	for (auto it : geometry) {                       //cycle through all objects and update them
		it->updateShape(dt);
	}
	for (auto it : transparentGeometry) {            //cycle through all transparent objects and update them
		it->updateShape(dt);
	}
	// Calculate FPS for output
	calculateFPS();
}

void Scene::render() {

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();
	
	// Set the camera
	cameras->applyCamera();
	//Apply lights
	for (auto it : lights) {
	it->applyLight();
	}
	// Render geometry/scene here -------------------------------------

	floor.render();
	for (auto it : geometry) {
		it->renderShapeWithShade();
	}
	for (auto it : transparentGeometry) {
		it->renderShape();
	}
	
	//after all renders, reset the flags 
	for (auto it : lights) {
		it->hasMoved = false;
	}
	for (auto it : geometry) {
		it->hasMoved = false;
	}
	for (auto it : transparentGeometry) {
		it->hasMoved = false;
	}
	// End render geometry --------------------------------------

	// Render text, should be last object rendered.
	renderTextOutput();
	
	// Swap buffers, after all objects are rendered.
	glutSwapBuffers();
}

void Scene::initialiseOpenGL()
{
	//OpenGL settings
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);			// Cornflour Blue Background (Just no!!!)
	glClearDepth(1.0f);									// Depth Buffer Setup
	glClearStencil(0);									// Clear stencil buffer
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
}

// Handles the resize of the window. If the window changes size the perspective matrix requires re-calculation to match new window size.
void Scene::resize(int w, int h) 
{
	width = w;
	height = h;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = (float)w / (float)h;
	fov = 45.0f;
	nearPlane = 0.1f;
	farPlane = 100.0f;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(fov, ratio, nearPlane, farPlane);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void Scene::renderShadowScene()
{
	Colour shade = { 0,0,0,0.6 };
	backWall.renderInShade(shade);
	woodEdge.renderInShade(shade);
	sideTableL.renderInShade(shade);
	sideTableR.renderInShade(shade);
}

// Calculates FPS
void Scene::calculateFPS()
{
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);

	if (time - timebase > 1000) {
		sprintf_s(fps, "FPS: %4.2f", frame*1000.0 / (time - timebase));
		timebase = time;
		frame = 0;
	}
}

//steps to set up an object, encapsulated the function calls for cleaner code, adds the object to the object array
void Scene::setupObject(ProceduralShape* object, Vertex scaling, Rotator rotation, std::string materialName, std::string textureName, bool globalRotation)
{
	setupObjectNoArray(object, scaling, rotation, materialName, textureName, globalRotation);
	geometry.push_back(std::shared_ptr<ProceduralShape>(object));
}

//similar to standard setup, just enables transprency and renders it later using the transparent object container
void Scene::setupTransparentObject(ProceduralShape* object, Vertex scaling, Rotator rotation, std::string materialName, std::string textureName, bool globalRotation)
{
	setupObjectNoArray(object, scaling, rotation, materialName, textureName, globalRotation);
	transparentGeometry.push_back(std::shared_ptr<ProceduralShape>(object));
	object->setTransparency(true);
}

//sets up the object, does not put it into any arrays, gets all the required settings and assets and assigns them to the objects
void Scene::setupObjectNoArray(ProceduralShape* object, Vertex scaling, Rotator rotation, std::string materialName, std::string textureName, bool globalRotation)
{
	object->setScale(scaling.x, scaling.y, scaling.z); 
	object->setRotation(rotation);
	if (materialName != "0") {
		object->setMaterial(library.getMaterial(materialName));
	}
	if (textureName != "0") {
		object->setTexture(library.getTexture(textureName));
	}
	object->setGlobalRotation(globalRotation);
}

//sets up the cloth pile, specific as the clothes have all the same texture and same manner of generation. doesnt do it with shadows
void Scene::setupClothPile(Model* pile, int num, DetailedCube* parent, Vertex relativePosition, Rotator rotation, Vertex scaling, std::string materialName)
{
	setupClothPileNoShadow(pile, num, parent, relativePosition, rotation, scaling, materialName);
	pile->setShadowImpostor(library.getTexture("SquareImposter"), scaling.y*0.75, nullptr, parent->getSide(0));
}

//sets up the cloth pile, encapsulation purposes, adds the cloth pile to the table as a child, also makes it create copies of itself, casts shadow
void Scene::setupClothPileNoShadow(Model* pile, int num, ProceduralShape* parent, Vertex relativePosition, Rotator rotation, Vertex scaling, std::string materialName)
{
	setupObjectNoArray(pile, { scaling.x,scaling.y,scaling.z }, rotation, materialName, "Fabric", false);
	pile->setCenter(relativePosition);
	pile->load("models/foldedCloth.obj");
	pile->createArrayOfSelf(num-1, { 0,0.0605f*scaling.x,0 });
	parent->addChild(pile);
}

//encapsulated all the initialisations for clearer search through the list, order is the render order as well (indentation indicates children)
void Scene::initialiseObjects()
{

	//front wall
	setupObject(&frontWoodPanel, { 2 * 2, 2.5 * 2, 0.15 * 2 }, { 0,0,0 }, "Wood", "Wood", false);
	setupObject(&backWall, { 1,1,1 }, { -90,0,0 }, "GreyShinyWall", "0", true);
	setupObject(&blueBackgroundFL, { 1,1,1 }, { -90,0,0 }, "BlueEmissive", "0", false);
	setupObject(&blueBackgroundFR, { 1,1,1 }, { -90,0,0 }, "BlueEmissive", "0", false);
	setupObject(&logoPanelL, { 0.56*2,0.56*2,0.075*2 }, { 0,180,0 }, "WhiteShine", "BoxBrand1", false);
	logoPanelL.setShadowImpostor(library.getTexture("SquareImposterB"), 2, nullptr, &backWall);
	setupObject(&logoPanelR, { 0.56*2,0.56*2,0.075*2 }, { 0,180,0 }, "WhiteShine", "BoxBrand2", false);
	logoPanelR.setShadowImpostor(library.getTexture("SquareImposterB"), 2, nullptr, &backWall);
	camineuSign.load("models/camineuSign.obj", true);
	setupObject(&camineuSign, { 1,1,1 }, { 0,-90,0 }, "WhiteShine", "0", false);
	sideTableL.load("models/sideShelf.obj");
	setupObject(&sideTableL, { 1,1,1 }, { 0,-90,0 }, "WhiteShine", "0", true);
		setupClothPileNoShadow(&clothesSL1, 6, &sideTableL, { -0.05,1.082,-3.05 }, { 0,0,0 }, { 1,1,1 }, "Cloth7");
		setupClothPileNoShadow(&clothesSL2, 4, &sideTableL, { -0.05,1.082,-2.6 }, { 0,0,0 }, { 1,1,1 }, "Cloth8");
		setupClothPileNoShadow(&clothesSL3, 8, &sideTableL, { 0.13,1.082,-2.2 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth7");
		setupClothPileNoShadow(&clothesSL4, 3, &sideTableL, { -0.24,1.082,-2.2 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth9");
		setupClothPileNoShadow(&clothesSL5, 2, &sideTableL, { -0.05,1.082,-1.76 }, { 0,0,0 }, { 1,1,1 }, "Cloth9");
		setupClothPileNoShadow(&clothesSL6, 8, &sideTableL, { -0.05,1.082,-1.32 }, { 0,0,0 }, { 1,1,1 }, "Cloth7");
		setupClothPileNoShadow(&clothesSL7, 8, &sideTableL, { 0.13,1.082,-0.86 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth8");
		setupClothPileNoShadow(&clothesSL8, 3, &sideTableL, { -0.24,1.082,-0.86 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth7");
	setupObject(&LED_L, { 0.054 * 2,0.017 * 2,0.054 * 2 }, { 0,0,0 }, "LED", "0", false);
	LED_L.createArrayOfSelf(3, { -5 * 2 * 0.054,0,0 });
	sideTableR.load("models/sideShelf.obj");
	setupObject(&sideTableR, { 1,1,1 }, { 0,90,0 }, "WhiteShine", "0", true);
		setupClothPileNoShadow(&clothesSR1, 8, &sideTableR, { -0.05,1.082,-0.96 }, { 0,0,0 }, { 1,1,1 }, "Cloth0");
		setupClothPileNoShadow(&clothesSR2, 8, &sideTableR, { -0.24,1.082,-1.42 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth5");
		setupClothPileNoShadow(&clothesSR3, 3, &sideTableR, { 0.13,1.082,-1.42 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth0");
		setupClothPileNoShadow(&clothesSR4, 9, &sideTableR, { -0.05,1.082,-1.89 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth2");
		setupClothPileNoShadow(&clothesSR5, 4, &sideTableR, { -0.24,1.082,-2.33 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth1");
		setupClothPileNoShadow(&clothesSR6, 1, &sideTableR, { 0.13,1.082,-2.33 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth6");
		setupClothPileNoShadow(&clothesSR7, 12, &sideTableR, { -0.05,1.082,-2.78 }, { 0,0,0 }, { 1,1,1 }, "Cloth4");
		setupClothPileNoShadow(&clothesSR8, 6, &sideTableR, { -0.24,1.082,-3.24 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth4");
		setupClothPileNoShadow(&clothesSR9, 3, &sideTableR, { 0.13,1.082,-3.24 }, { 0,0,0 }, { clothS,clothS,clothS }, "Cloth3");
	setupObject(&LED_R, { 0.054 * 2,0.017 * 2,0.054 * 2 }, { 0,0,0 }, "LED", "0", false);
	LED_R.createArrayOfSelf(3, { -5 * 2 * 0.054,0,0 });
	jacket.load("models/jacket.obj", true);
	setupObject(&jacket, { 1,1,1 }, { 0,-90,0 }, "WhiteWall", "Jacket", true);
	jacket.setShadowImpostor(library.getTexture("CircleImposter"), 2.2, nullptr, &backWall);
	vest1.load("models/vest.obj", true);
	setupObject(&vest1, { 1,1,1 }, { 0,-90,0 }, "Cloth6", "Fabric", true);
	vest1.setShadowImpostor(library.getTexture("CircleImposter"), 1.8, nullptr, &backWall);
	vest2.load("models/vest.obj", true);
	setupObject(&vest2, { 0.63,0.63,0.63 }, { 0,-90,0 }, "Cloth5", "Fabric", true);
	vest2.setShadowImpostor(library.getTexture("CircleImposter"), 1.1, nullptr, &backWall);
	vest3.load("models/vest.obj", true);
	setupObject(&vest3, { 0.63,0.63,0.63 }, { 0,-90,0 }, "Cloth3", "Fabric", true);
	vest3.setShadowImpostor(library.getTexture("CircleImposter"), 1.1, nullptr, &backWall);
	setupObject(&plantPotL, { 1,1,1 }, { 0,0,0 }, "WhiteSHine", "0", false);
	plantPotL.setShadowImpostor(library.getTexture("SquareImposter"), 1.4, 1.3, nullptr, &backWall);
	setupObject(&plantPotR, { 1,1,1 }, { 0,0,0 }, "WhiteSHine", "0", false);
	plantPotR.setShadowImpostor(library.getTexture("SquareImposter"), 1.4,1.3, nullptr, &backWall);

	//right wall
	setupObject(&pillarR1, { 0.5*2, 2.5*2, 0.5*2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&pillarR2, { 0.5*2, 2.5*2, 0.5*2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&pillarR3, { 0.5*2, 2.5*2, 0.5*2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&pillarR4, { 0.5*2, 2.5*2, 0.5*2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&overheadR1, { 0.5*2, 0.5*2, 2*2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&overheadR2, { 0.5*2, 0.5*2, 2*2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&seatR1, { 0.5*2, 0.5*2, 2*2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&seatR2, { 0.5*2, 0.5*2, 2*2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&backWallR, { 1,1,1 }, { 90,90,0 }, "Wood", "Wood", true);
	setupObject(&metalBarR1A, { 1,1,1 }, { 0,90,0 }, "Metal", "0", true);
	setupObject(&metalBarR1B, { 1,1,1 }, { 90,90,0 }, "Metal", "0", true);
	setupObject(&metalBarR2A, { 1,1,1 }, { 0,90,0 }, "Metal", "0", true);
	setupObject(&metalBarR2B, { 1,1,1 }, { 90,90,0 }, "Metal", "0", true);
	setupObject(&blueBackgroundSR, { 1,1,1 }, { 0,90,90 }, "BlueEmissive", "0", true);
	brandSignR1.load("models/brandSign.obj",true);
	setupObject(&brandSignR1, { 1,1,1 }, { 0,90,0 }, "SignBrand", "SignBrand3", true);
	brandSignR2.load("models/brandSign.obj",true);
	setupObject(&brandSignR2, { 1,1,1 }, { 0,90,0 }, "SignBrand", "SignBrand4", true);
	setupObject(&TshirtsTR, { 1,1,1 }, { -90,0,180 }, "WhiteShine", "ArticleBrand3", true);
	TshirtsTR.setTransparency(true);
	TshirtsTR.createArrayOfSelf(6, { 0,0.6,0 });
	setupObject(&TshirtsBR, { 1,1,1 }, { -90,0,180 }, "WhiteShine", "ArticleBrand4", true);
	TshirtsBR.setTransparency(true);
	TshirtsBR.createArrayOfSelf(8, { 0,0.4,0 });

	//left wall
	setupObject(&pillarL1, { 0.5 * 2, 2.5 * 2, 0.5 * 2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&pillarL2, { 0.5 * 2, 2.5 * 2, 0.5 * 2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&pillarL3, { 0.5 * 2, 2.5 * 2, 0.5 * 2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&pillarL4, { 0.5 * 2, 2.5 * 2, 0.5 * 2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&overheadL1, { 0.5 * 2, 0.5 * 2, 2 * 2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&overheadL2, { 0.5 * 2, 0.5 * 2, 2 * 2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&seatL1, { 0.5 * 2, 0.5 * 2, 2 * 2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&seatL2, { 0.5 * 2, 0.5 * 2, 2 * 2 }, { 0,0,0 }, "WhiteWall", "0", false);
	setupObject(&backWallL, { 1,1,1 }, { -90,90,0 }, "Wood", "Wood", true);
	setupObject(&metalBarL1A, { 1,1,1 }, { 0,90,0 }, "Metal", "0", true);
	setupObject(&metalBarL1B, { 1,1,1 }, { -90,90,0 }, "Metal", "0", true);
	setupObject(&metalBarL2A, { 1,1,1 }, { 0,90,0 }, "Metal", "0", true);
	setupObject(&metalBarL2B, { 1,1,1 }, { -90,90,0 }, "Metal", "0", true);
	setupObject(&blueBackgroundSL, { 1,1,1 }, { 0,90,90 }, "BlueEmissive", "0", true);
	brandSignL1.load("models/brandSign.obj",true);
	setupObject(&brandSignL1, { 1,1,1 }, { 0,-90,0 }, "SignBrand", "SignBrand1", true);
	brandSignL2.load("models/brandSign.obj",true);
	setupObject(&brandSignL2, { 1,1,1 }, { 0,-90,0 }, "SignBrand", "SignBrand2", true);
	setupObject(&TshirtsTL, { 1,1,1 }, { -90,0,180 }, "WhiteShine", "ArticleBrand1", true);
	TshirtsTL.setTransparency(true);
	TshirtsTL.createArrayOfSelf(11, { 0,0.3,0 });
	setupObject(&TshirtsBL, { 1,1,1 }, { -90,0,180 }, "WhiteShine", "ArticleBrand2", true);
	TshirtsBL.setTransparency(true);
	TshirtsBL.createArrayOfSelf(7, { 0,0.5,0 });

	//middle (maybe add random num,ber generator for piles)
	setupObject(&tableTL, { 0.836 * 2,0.522 * 2,0.836 * 2 }, { 0,0,0 }, "WhiteShine", "0", false);
	tableTL.setShadowImpostor(library.getTexture("SquareImposterA"), 3, nullptr, floor.getMirrorPlane());
	tableTL.setShadowProjection(&light0, floor.getMirrorPlane());
		setupClothPile(&clothesTLB1, 5, &tableTL, { 0.54,0.54,-0.453 }, { 0,90,0 }, { 1,1,1 }, "Cloth0");
		setupClothPile(&clothesTLB2, 6, &tableTL, { 0.108,0.54,-0.453 }, { 0,90,0 }, { 1,1,1 }, "Cloth1");
		setupClothPile(&clothesTLB3, 2, &tableTL, { -0.328,0.54,-0.453 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth2");
		setupClothPile(&clothesTLB4, 3, &tableTL, { -0.611,0.54,-0.453 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth0");
		setupClothPile(&clothesTLT1, 1, &tableTL, { 0.44,0.54,0.43 }, { 0,90,0 }, { 1,1,1 }, "Cloth0");
		setupClothPile(&clothesTLT2, 2, &tableTL, { 0,0.54,0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth2");
		setupClothPile(&clothesTLT3, 4, &tableTL, { -0.5,0.54,0.43 }, { 0,90,0 }, { 1,1,1 }, "Cloth3");
	setupObject(&footTL, { 0.692 * 2,0.093 * 2,0.692 * 2 }, { 0,0,0 }, "BlueEmissive", "0", false);
	setupObject(&tableTR, { 0.836 * 2,0.522 * 2,0.836 * 2 }, { 0,0,0 }, "WhiteShine", "0", false);
	tableTR.setShadowImpostor(library.getTexture("SquareImposterA"), 3, nullptr, floor.getMirrorPlane());
	tableTR.setShadowProjection(&light0, floor.getMirrorPlane());
		setupClothPile(&clothesTRB1, 5, &tableTR, { 0.43,0.54,-0.453 }, { 0,90,0 }, { 1,1,1 }, "Cloth0");
		setupClothPile(&clothesTRB2, 6, &tableTR, {0,0.54,-0.453}, { 0,90,0 }, { 1,1,1 }, "Cloth1");
		setupClothPile(&clothesTRB3, 4, &tableTR, { -0.44,0.54,-0.453 }, { 0,90,0 }, { 1,1,1 }, "Cloth3");
		setupClothPile(&clothesTRT1, 2, &tableTR, { 0.5,0.54,0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth2");
		setupClothPile(&clothesTRT2, 4, &tableTR, { 0,0.54,0.43 }, { 0,90,0 }, { 1,1,1 }, "Cloth1");
		setupClothPile(&clothesTRT3, 1, &tableTR, { -0.48,0.54,0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth5");
	setupObject(&footTR, { 0.692 * 2,0.093 * 2,0.692 * 2 }, { 0,0,0 }, "BlueEmissive", "0", false);
	setupObject(&tableBL, { 0.836 * 2,0.522 * 2,0.836 * 2 }, { 0,0,0 }, "WhiteShine", "0", false);
	tableBL.setShadowImpostor(library.getTexture("SquareImposterA"), 3, nullptr, floor.getMirrorPlane());
	tableBL.setShadowProjection(&light0, floor.getMirrorPlane());
		setupClothPile(&clothesBLB1, 7, &tableBL, { 0.54,0.54,-0.43 }, { 0,90,0 }, { 1,1,1 }, "Cloth4");
		setupClothPile(&clothesBLB2, 2, &tableBL, { 0.08,0.54,-0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth5");
		setupClothPile(&clothesBLB3, 5, &tableBL, { -0.27,0.54,-0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth6");
		setupClothPile(&clothesBLB4, 6, &tableBL, { -0.62,0.54,-0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth0");
		setupClothPile(&clothesBLT1, 3, &tableBL, { 0.33,0.54,0.45 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth2");
		setupClothPile(&clothesBLT2, 2, &tableBL, { -0.31,0.54,0.45 }, { 0,90,0 }, { 1,1,1 }, "Cloth5");
	setupObject(&footBL, { 0.692 * 2,0.093 * 2,0.692 * 2 }, { 0,0,0 }, "BlueEmissive", "0", false);
	setupObject(&tableBR, { 0.836 * 2,0.522 * 2,0.836 * 2 }, { 0,0,0 }, "WhiteShine", "0", false);
	tableBR.setShadowImpostor(library.getTexture("SquareImposterA"), 3, nullptr, floor.getMirrorPlane());
	tableBR.setShadowProjection(&light0, floor.getMirrorPlane());
		setupClothPile(&clothesBRB1, 8, &tableBR, { 0.54,0.54,-0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth0");
		setupClothPile(&clothesBRB2, 2, &tableBR, { 0.19,0.54,-0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth4");
		setupClothPile(&clothesBRB3, 5, &tableBR, { -0.22,0.54,-0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth6");
		setupClothPile(&clothesBRB4, 7, &tableBR, { -0.55,0.54,-0.43 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth2");
		setupClothPile(&clothesBRT1, 3, &tableBR, { 0.56,0.54,0.45 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth2");
		setupClothPile(&clothesBRT2, 2, &tableBR, { 0.2,0.54,0.61 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth1");
		setupClothPile(&clothesBRT3, 6, &tableBR, { 0.2,0.54,0.23 }, { 0,90,0 }, { clothS,clothS,clothS }, "Cloth6");
		setupClothPile(&clothesBRT4, 2, &tableBR, { -0.32,0.54,0.45 }, { 0,90,0 }, { 1,1,1 }, "Cloth3");
	setupObject(&footBR, { 0.692 * 2,0.093 * 2,0.692 * 2 }, { 0,0,0 }, "BlueEmissive", "0", false);
	frontTable.load("models/frontTable.obj");
	setupObject(&frontTable, { 1,1,1 }, { 0,90,0 }, "WhiteShine", "0", true);
	frontTable.setShadowImpostor(library.getTexture("SquareImposter"), 7, 2, nullptr, floor.getMirrorPlane(),false);
	setupObject(&miniPlantPot, { 1,1,1 }, { 0,0,0 }, "WhiteSHine", "0", false);
	miniPlantPot.setShadowImpostor(library.getTexture("CircleImposter"),0.4, nullptr, &D_FrontTable);
	woodEdge.load("models/woodedEdge.obj");
	setupObject(&woodEdge, { 1,1,1 }, { 0,-90,0 }, "Wood", "Wood", true);

	//shadow dummies
	setupObject(&D_SideTableLB, { 1,1,1 }, { 0,0,0 }, "WhiteShine", "0", false);
	D_SideTableLB.setShadowImpostor(library.getTexture("SquareImposter"), 8,2.5, nullptr, floor.getMirrorPlane());
	setupObject(&D_SideTableLT, { 1,1,1 }, { 0,0,0 }, "WhiteShine", "0", false);
	D_SideTableLT.setShadowImpostor(library.getTexture("SquareImposter"), 3.5, 1.2, nullptr, &D_SideTableLB);
	setupObject(&D_SideTableRB, { 1,1,1 }, { 0,0,0 }, "WhiteShine", "0", false);
	D_SideTableRB.setShadowImpostor(library.getTexture("SquareImposter"), 8, 2.5, nullptr, floor.getMirrorPlane());
	setupObject(&D_SideTableRT, { 1,1,1 }, { 0,0,0 }, "WhiteShine", "0", false);
	D_SideTableRT.setShadowImpostor(library.getTexture("SquareImposter"), 3.5, 1.2, nullptr, &D_SideTableLB);

	// transparent objects 
	setupTransparentObject(&plantL, { 1,1,1 }, plantL.getRotation(), "Screen", "Plant", true);
	plantL.setActiveCamera(cameras->getActivePointer());
	setupTransparentObject(&plantR, { 1,1,1 }, plantR.getRotation(), "Screen", "Plant", true);
	plantR.setActiveCamera(cameras->getActivePointer());
	setupTransparentObject(&miniPlant, { 1,1,1 }, miniPlant.getRotation(), "Screen", "Plant", true);
	miniPlant.setActiveCamera(cameras->getActivePointer());
	setupTransparentObject(&monitorL, { 1,1,1 }, { 55.6,-30.6,0 }, "Screen", "Screen", true);
		setupObjectNoArray(&monitorLS1, { 1,1,1 }, { 0,0,0 }, "Screen", "ScreenA", true);
		monitorLS1.setTransparency(true);
		monitorL.addChild(&monitorLS1);
		setupObjectNoArray(&monitorLS2, { 1,1,1 }, { 0,0,0 }, "Screen", "ScreenA", true);
		monitorLS2.setTransparency(true);
		monitorL.addChild(&monitorLS2);
	setupTransparentObject(&monitorR, { 1,1,1 }, { 55.6,0,0 }, "Screen", "Screen", true);
		setupObjectNoArray(&monitorRS1, { 1,1,1 }, { 0,0,0 }, "Screen", "ScreenA", true);
		monitorRS1.setTransparency(true);
		monitorR.addChild(&monitorRS1);
		setupObjectNoArray(&monitorRS2, { 1,1,1 }, { 0,0,0 }, "Screen", "ScreenA", true);
		monitorRS2.setTransparency(true);
		monitorR.addChild(&monitorRS2);
	setupTransparentObject(&keyboard, { 1,1,1 }, { 0,0,0 }, "Screen", "Screen", false);
	setupTransparentObject(&signTLT, { 1,1,1 }, { 72.2,0,0 }, "Screen", "Screen", false);
	signTLT.createArrayOfSelf(1, { -0.70,0,0 });
	setupTransparentObject(&signTLB, { 1,1,1 }, { 107,0,0 }, "Screen", "Screen", false);
	signTLB.createArrayOfSelf(1, { -0.70,0,0 });
	setupTransparentObject(&signBLT, { 1,1,1 }, { 72.2,0,0 }, "Screen", "Screen", false);
	signBLT.createArrayOfSelf(1, { -0.70,0,0 });
	setupTransparentObject(&signBLB, { 1,1,1 }, { 107,0,0 }, "Screen", "Screen", false);
	signBLB.createArrayOfSelf(1, { -0.70,0,0 });
	setupTransparentObject(&signTRT, { 1,1,1 }, { 72.2,0,0 }, "Screen", "Screen", false);
	signTRT.createArrayOfSelf(1, { -0.70,0,0 });
	setupTransparentObject(&signTRB, { 1,1,1 }, { 107,0,0 }, "Screen", "Screen", false);
	signTRB.createArrayOfSelf(1, { -0.70,0,0 });
	setupTransparentObject(&signBRT, { 1,1,1 }, { 72.2,0,0 }, "Screen", "Screen", false);
	signBRT.createArrayOfSelf(1, { -0.70,0,0 });
	setupTransparentObject(&signBRB, { 1,1,1 }, { 107,0,0 }, "Screen", "Screen", false);
	signBRB.createArrayOfSelf(1, { -0.70,0,0 });
	
	// reflective floor
	floor.getMirrorPlane()->setMaterial(library.getMaterial("Floor"));
	floor.getMirrorPlane()->setTexture(library.getTexture("Floor"));
	floor.getMirrorPlane()->setTransparency(true);
	floor.getMirrorPlane()->setUVmultiplier(4,4);
	floor.addObjectArray(geometry);
}

// Compiles standard output text including FPS and current mouse position.
void Scene::renderTextOutput()
{
	// Render current mouse position and frames per second.
	sprintf_s(mouseText, "Mouse: %i, %i", input->getMouseX(), input->getMouseY());
	displayText(-1.f, 0.96f, 1.f, 0.f, 0.f, mouseText);
	displayText(-1.f, 0.90f, 1.f, 0.f, 0.f, fps);
}

// Renders text to screen. Must be called last in render function (before swap buffers)
void Scene::displayText(float x, float y, float r, float g, float b, char* string) {
	// Get Lenth of string
	int j = strlen(string);

	// Swap to 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Orthographic lookAt (along the z-axis).
	gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Set text colour and position.
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	// Render text.
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
	// Reset colour to white.
	glColor3f(1.f, 1.f, 1.f);

	// Swap back to 3D rendering.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, ((float)width/(float)height), nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
}
