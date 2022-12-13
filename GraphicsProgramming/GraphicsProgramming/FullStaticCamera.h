#pragma once
#include "Camera.h"

//camera subclass with no controls
class FullStaticCamera :
    public Camera
{
public:
    //camera constructor values are forwarded to the base construcor
    FullStaticCamera(Vertex position, Rotator rotation, int ID, int* screen_width, int* screen_height) : Camera(position, rotation, ID, screen_width, screen_height) {} 
    void handleInput(float dt) override;         //override of handleInput() with more limited version
};

