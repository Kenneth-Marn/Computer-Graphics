#ifndef SHADOW_H
#define SHADOW_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Viewer.h"
#include "Skybox.h"
#include "../Sphere.hpp"
#include "../HeightMap.h"

#define M_PI           3.14159265358979323846
using namespace std;

class Shadow
{
    public:
        //Functions
        Shadow();
        Shadow(int,int);
        Shadow(int,int,float xlight,float ylight,float zlight);
        int createSphereVAO();
        int setup(unsigned int ProgramID,int& winX, int& winY,float camZ);
        int createSquareVAO();
        void drawScene(int shadow, int ProgramID, HeightMap *Terrain,Viewer *Camera,float camZ);
        void getPVLightMatrix(glm::mat4 *projection, glm::mat4 *viewMatrix);
        void reshape(int width, int height,int ProgramID,float camZ);
        void setLightX(float);
        void setLightZ(float);
        void setLightY(float);
        //Variables
        // This program makes use of the Sphere class, that constructs a sphere mesh.
        Sphere *sphere;
        unsigned int vaoHandlers[3];
        unsigned int texSz = 1024*4;

        float lightX = 0.0f;
        float lightY = 2.0f;
        float lightZ = -0.0f;

        float positionSpheres = -10.0f;
        float rotationSpheres = 0.0f;

        int winX = 500;
        int winY = 500;

        // Frame buffer object handles
        GLuint FramebufferName, depthTexture, depth_rb;
        virtual ~Shadow();
    protected:
    private:
};

#endif // SHADOW_H
