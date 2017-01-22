#ifndef SKYBOX_H
#define SKYBOX_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;
class Skybox
{
    public:
        Skybox();
        int createCubeVAO();
        unsigned int createCubeMap(int);
        unsigned int* createMultCubeMap(unsigned int);
        void renderSkybox(int programID,float xPos,float yPos,float zPos,int handle);
        void renderTable(int cubeID,float xPos, float yPos, float zPos, int vaoHandle);

        unsigned int textures[2];
        virtual ~Skybox();
    protected:
    private:
};

#endif // SKYBOX_H
