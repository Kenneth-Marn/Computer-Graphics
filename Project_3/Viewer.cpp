
#include <stdio.h>
#include <math.h>

#include "Viewer.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
using namespace std;
#define DEG2RAD(x) ((x)*M_PI/180.0)
#define RAD2DEG(x) ((x)*180.0/M_PI)

Viewer::Viewer( glm::vec3 eye, glm::vec3 at, glm::vec3 up )
{
    initEye = eye;
    initAt = at;
    initUp = up;
    
    reset();
}

Viewer::~Viewer(){}

/**
 Assumes the current matrix has been calculated (usually in
 update() or reset())
 */
const glm::mat4 Viewer::getViewMtx() const
{
    return viewMtx;
}

/**
 Resets the view matrix to the value the camera was
 initialised with
 */
void Viewer::reset()
{
    viewMtx = glm::lookAt(initEye, initAt, initUp);
}

glm::vec3 ObjectViewer::rotx(glm::vec3 v, float a)
{
    return glm::vec3(v.x, v.y*cos(a) - v.z*sin(a), v.y*sin(a) + v.z*cos(a));
}

glm::vec3 ObjectViewer::roty(glm::vec3 v, float a)
{
    return glm::vec3(v.x*cos(a) + v.z*sin(a), v.y, -v.x*sin(a) + v.z*cos(a));
}

glm::vec3 ObjectViewer::rotz(glm::vec3 v, float a)
{
    return glm::vec3(v.x*cos(a) - v.y*sin(a), v.x*sin(a) + v.y*cos(a), v.z);
}

ObjectViewer::ObjectViewer(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
: Viewer(eye, at, up)
{
    scaling = 1.f;
    angle_x = 0.f;
    angle_y = 0.f;
}

void ObjectViewer::update( InputState input )
{
    if ( input.lMousePressed )
    {
        // The first 3 rows of the view matrix are the camera x, y, z axes
        // in world coordinate space. (see lecture 6)
        glm::vec3 eyeX( viewMtx[0][0], viewMtx[1][0], viewMtx[2][0] );
        glm::vec3 eyeY( viewMtx[0][1], viewMtx[1][1], viewMtx[2][1] );
        glm::vec3 eyeZ( viewMtx[0][2], viewMtx[1][2], viewMtx[2][2] );
        
        float yRot = input.deltaX;
        float xRot = input.deltaY;
        
        // Rotate about the eye's y axis.
        if ( yRot != 0 )
        {
            float sinY = sin(DEG2RAD(yRot));
            float cosY = cos(DEG2RAD(yRot));
            
            glm::vec3 tmpX = eyeX;
            eyeX = cosY*tmpX + sinY*eyeZ;
            eyeZ = -sinY*tmpX + cosY*eyeZ;
        }
        // Rotate about the eye's x axis.
        if ( xRot != 0 )
        {
            float sinX = sin(DEG2RAD(xRot));
            float cosX = cos(DEG2RAD(xRot));
            
            glm::vec3 tmpY = eyeY;
            eyeY = cosX*tmpY - sinX*eyeZ;
            eyeZ = sinX*tmpY + cosX*eyeZ;
        }
        
        // Update the view matrix with new eye axes.
        viewMtx[0][0] = eyeX[0];
        viewMtx[1][0] = eyeX[1];
        viewMtx[2][0] = eyeX[2];
        
        viewMtx[0][1] = eyeY[0];
        viewMtx[1][1] = eyeY[1];
        viewMtx[2][1] = eyeY[2];
        
        viewMtx[0][2] = eyeZ[0];
        viewMtx[1][2] = eyeZ[1];
        viewMtx[2][2] = eyeZ[2];
    }
    
    else if ( input.rMousePressed )
    {
        scaling -= scaling*(input.deltaY / 100.0);
        if (scaling < 0.1f)
            scaling = 0.1f;
        else if (scaling > 100.f)
            scaling = 100.f;
    }
}

void ObjectViewer::moveFB(float speed)
{
    
    viewMtx[3][2] += speed;
}

void ObjectViewer::moveLR(float speed)
{
    
   // glm::vec3 eyeY( viewMtx[0][1], viewMtx[1][1], viewMtx[2][1] );
   // viewMtx = glm::rotate(viewMtx, float(-DEG2RAD(speed)*5), glm::vec3(eyeY[0], eyeY[1], eyeY[2]));
     viewMtx[3][0] -= speed;
}

/**
 Again, you almost certainly DON'T want to use this camera!
 */
DodgyObjectViewer::DodgyObjectViewer(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
: Viewer(eye, at, up)
{
    xRot = 0.0f;
    yRot = 0.0f;
}

void DodgyObjectViewer::update( InputState input )
{
    if ( input.lMousePressed )
    {
        // Update rotation about world x and y axes
        yRot += input.deltaX;
        xRot += input.deltaY;
        
        // Use lookAt to compute new view matrix for us
        viewMtx = glm::lookAt( initEye, initAt, initUp );
        viewMtx = glm::rotate( viewMtx, yRot, glm::vec3(0.0f, 1.0f, 0.0f) );
        viewMtx = glm::rotate( viewMtx, xRot, glm::vec3(1.0f, 0.0f, 0.0f) );
    }
}



