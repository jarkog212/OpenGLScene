#pragma once
#include "Camera.h"
//static camera class, limited control
class StaticCamera :
    public Camera
{
public:
    StaticCamera(Vertex position, Rotator rotation, int ID, int* screen_width, int* screen_height) : Camera(position, rotation, ID, screen_width, screen_height) {}  //forwards initialisation to the base class
    void handleInput(float dt) override;      // overrides base class handle input with a more limited version
};

