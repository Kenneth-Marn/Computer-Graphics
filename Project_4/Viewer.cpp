
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "Viewer.h"
#include "glm/gtc/matrix_transform.hpp"

#define M_PI 3.142
#define DEG2RAD(x) ((x)*M_PI/180.0)
#define RAD2DEG(x) ((x)*180.0/M_PI)


using namespace std;
Viewer::Viewer( glm::vec3 eye )
{
    initEye = eye;
    //Note: The 'z' value of initAt can be considered as the "range" of your viewing
    currentXAngle = 0.01f;
    currentYAngle = 0.0f;
    currentZAngle = 0.0f; // - 1 to face the front

    initAt=glm::vec3 (currentXAngle, currentYAngle, currentZAngle);
    initUp=glm::vec3 (0.0f, 1.0f, 0.0f);
    viewMtx = glm::lookAt(initEye, initAt, initUp);
    currDegreeXaxe=0;
    currDegreeYaxe=0;
    reset();
}

Viewer::~Viewer(){}
/**
*   Initialize position
*/
void Viewer::init(float x,float y, float z){
    currentX=x;
    currentY=y;
    currentZ=z;
}

/**When we change the camera position, we make sure the changing is based
*  on where it looks at.
*/
void Viewer::setCamPos( float x,float y, float z){
    currentX=currentX+x;
    currentY=currentY+y;
    currentZ=currentZ+z;
    //initEye = glm::vec3(x,y,z);
    //glm::vec3 at(0.0f, 0.0f, 0.0f);
    //glm::vec3 up(0.0f, 1.0f, 0.0f);
    //viewMtx = glm::lookAt(initEye, initAt, initUp);

    initEye = glm::vec3(currentX,currentY,currentZ);
    //viewMtx =glm::translate(viewMtx,glm::vec3(x,y,z));
    //cout<<"x,y,z pos: "<<currentX<<","<<currentY<<","<<currentZ<<endl;
    reset();
}
/**
*   Sets up and update the coordinate on where it looks at
*/
void Viewer::setLookAt(float x,float y, float z){
    currentXAngle=currentXAngle+x;
    currentYAngle=currentYAngle+y;
    currentZAngle=currentZAngle+z;
    initAt=glm::vec3(currentXAngle,currentYAngle,currentZAngle);
    viewMtx = glm::lookAt(initEye, initAt, initUp);
    //cout<<"x,y,z angles: "<<currentXAngle<<","<<currentYAngle<<","<<currentZAngle<<endl;
    reset();
}
/**
*   Gets the degree of X-axis rotation
*/
float Viewer::getCurrentDegreeInXAxis(){
    return currDegreeXaxe;
}
/**
*   Gets the degree of Y-axis rotation
*/
float Viewer::getCurrentDegreeInYAxis(){
    return currDegreeYaxe;
}
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
 initialised with. Assumes looking at the origin.
*/
void Viewer::reset()
{
    //glm::vec3 at(0.0f, 0.0f, 0.0f);
    //glm::vec3 up(0.0f, 1.0f, 0.0f);
    //viewMtx = glm::lookAt(initEye, at, up);
    viewMtx = glm::lookAt(initEye, initAt, initUp);
}

ObjectViewer::ObjectViewer(glm::vec3 eye)
    : Viewer(eye)
{}

void ObjectViewer::update( InputState &input )
{
    float xRot, yRot;
    input.readDeltaAndReset( &yRot, &xRot );
    //cout<<xRot<<","<<yRot<<endl;
    //if ( input.lMousePressed )
    //{

        // The first 3 rows of the view matrix are the camera x, y, z axes
        // in world coordinate space. (see lecture 6)
        glm::vec3 eyeX( viewMtx[0][0], viewMtx[1][0], viewMtx[2][0] );
        glm::vec3 eyeY( viewMtx[0][1], viewMtx[1][1], viewMtx[2][1] );
        glm::vec3 eyeZ( viewMtx[0][2], viewMtx[1][2], viewMtx[2][2] );

        // Rotate about the eye's y axis.
        xRot=xRot*0.5f;
        if ( yRot != 0 )
        {
            currDegreeXaxe=currDegreeXaxe+yRot;
            //Resets degree if above 360 or below 0
            if(currDegreeXaxe>=360){
                currDegreeXaxe=(int)currDegreeXaxe%360;
            }
            if(currDegreeXaxe<0){
                currDegreeXaxe=360-((int)abs(currDegreeXaxe)%360);
            }
            //currentXAngle = currentXAngle+yRot;
            //currentZAngle = currentZAngle+yRot;
            currentXAngle = currentX+cos(M_PI*currDegreeXaxe/180);
            currentZAngle = currentZ+sin(M_PI*currDegreeXaxe/180); //-1.0f to look at the front
            initAt = glm::vec3(currentXAngle,currentYAngle,currentZAngle);

            float sinY = sin(DEG2RAD(yRot));
            float cosY = cos(DEG2RAD(yRot));
            //viewMtx =glm::rotate(viewMtx,sinY,glm::vec3(0.0f, yRot, 0.0f));
            //viewMtx =glm::translate(viewMtx,glm::vec3(0.0f, yRot, 0.0f));
            /*glm::vec3 tmpX = eyeX;
            eyeX = cosY*tmpX + sinY*eyeZ;
            eyeZ = -sinY*tmpX + cosY*eyeZ;*/
        }
        // Rotate about the eye's x axis.
        if ( xRot != 0 )
        {
            if((currDegreeYaxe+xRot)<270 &&currDegreeYaxe >90 ){
                currDegreeYaxe=270;
            }
            else if((currDegreeYaxe+xRot)>90 &&currDegreeYaxe <270){
                currDegreeYaxe=90;
            }else{
                currDegreeYaxe=currDegreeYaxe+xRot;
            }
            //Resets degree if above 360 or below 0
            if(currDegreeYaxe>=360){
                currDegreeYaxe=(int)currDegreeYaxe%360;
            }
            if(currDegreeYaxe<0){
                currDegreeYaxe=360-((int)abs(currDegreeYaxe)%360);
            }
//            if(currDegreeYaxe>90 && currDegreeYaxe<270){
//                currDegreeYaxe=90;
//            }
//            else if(currDegreeYaxe<270){
//                currDegreeYaxe=270;
//            }
            currentYAngle = currentY+sin(M_PI*currDegreeYaxe/180);
            //currentZAngle = currentZ+cos(M_PI*currDegreeYaxe/180); //-1.0f to look at the front
            initAt = glm::vec3(currentXAngle,currentYAngle,currentZAngle);
            //viewMtx =glm::translate(viewMtx,glm::vec3(xRot, 0.0f, 0.0f));
            /*float sinX = sin(DEG2RAD(xRot));
            float cosX = cos(DEG2RAD(xRot));

            glm::vec3 tmpY = eyeY;
            eyeY = cosX*tmpY - sinX*eyeZ;
            eyeZ = sinX*tmpY + cosX*eyeZ;*/
        }

        // Update the view matrix with new eye axes.
        /*viewMtx[0][0] = eyeX[0];
        viewMtx[1][0] = eyeX[1];
        viewMtx[2][0] = eyeX[2];

        viewMtx[0][1] = eyeY[0];
        viewMtx[1][1] = eyeY[1];
        viewMtx[2][1] = eyeY[2];

        viewMtx[0][2] = eyeZ[0];
        viewMtx[1][2] = eyeZ[1];
        viewMtx[2][2] = eyeZ[2];*/
        reset();
    //}
}

/**
   Again, you almost certainly DON'T want to use this camera!
 */
WorldObjectViewer::WorldObjectViewer(glm::vec3 eye)
    : Viewer(eye)
{
    //xRot = 0.0f;//original
    //yRot = 0.0f;//original
    xRot = 0.0f;
    yRot = 0.0f;
}

void WorldObjectViewer::update( InputState &input )
{
    float x, y;
    input.readDeltaAndReset(&x, &y);

    if ( input.lMousePressed )
    {
        // Update rotation about world x and y axes
        yRot += 0.01f * x;
        xRot += 0.01f * y;

        // Rotate from our original camera position based on mouse motion
        reset();
        viewMtx = glm::rotate( viewMtx, yRot, glm::vec3(0.0f, 1.0f, 0.0f) );
        viewMtx = glm::rotate( viewMtx, xRot, glm::vec3(1.0f, 0.0f, 0.0f) );
    }
}

// void Viewer::orthogonaliseViewMtx()
// {
//     Vec3 xAxis( viewMtx[0], viewMtx[1], viewMtx[2] );
//     Vec3 yAxis( viewMtx[4], viewMtx[5], viewMtx[6] );
//     Vec3 zAxis( viewMtx[8], viewMtx[9], viewMtx[10] );

//     // Gram-Schmidt orthogonalisation
//     normalise( yAxis );
//     zAxis = zAxis - dot( zAxis, yAxis ) * yAxis;
//     normalise( zAxis );
//     xAxis = xAxis - dot( xAxis, yAxis ) * yAxis
//                   - dot( xAxis, zAxis ) * zAxis;
//     normalise( xAxis );

//     viewMtx[0] = xAxis[0]; viewMtx[4] = yAxis[0]; viewMtx[8] = zAxis[0];
//     viewMtx[1] = xAxis[1]; viewMtx[5] = yAxis[1]; viewMtx[9] = zAxis[1];
//     viewMtx[2] = xAxis[2]; viewMtx[6] = yAxis[2]; viewMtx[10] = zAxis[2];
// }


