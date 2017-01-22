#ifndef SUNMOON_H
#define SUNMOON_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Sphere.hpp"
#include "../stb_image.h"
#include "../Shader.hpp"
#include "../shader.h"
#include "../Viewer.h"

#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_TEX 2
#define LIGHT_SPEED 0.2f
#define NUM_TEXTURES 2

#define M_PI           3.14159265358979323846
#define M_PI_2           M_PI/2
using namespace std;
class SunMoon
{
    public:
        SunMoon();
        SunMoon(int winW,int winH,const char* path,int identifier);
        int createSphereVAO();
        void loadRGBTexture(const char *path);
        int createTextures();
        void renderSunMoon(unsigned int ProgramID,float xPos, float yPos, float zPos,double dt);
        int setupShader(unsigned int id,int winW, int winH,float camZ);

        //New functions
        void RenderCube();
        void renderTwo(Viewer *Camera,Shader shader,Shader hdrShader,float camZ,float xPos, float yPos, float zPos);
        void RenderQuad();
        GLuint loadTexture(GLchar const * path);
        void setupTexture(float winW,float winH);
        void setup_Texture();


        //Variables
        unsigned int sunMoonVaoHandle;
        int object=0; //0 for sun and 1 for moon
        GLuint TexID[NUM_TEXTURES];
        Sphere* sunMoon;
        float rotation = 0.0f;
        float in_rotation=0.0f;

        //New vars
        GLuint woodTexture;
        GLuint colorBuffer;
        GLuint hdrFBO;
        GLuint rboDepth;
        int sunOrMoon;
        vector<glm::vec3> lightPositions;
        std::vector<glm::vec3> lightColors;

        int winW;
        int winH;
        virtual ~SunMoon();
    protected:
    private:
};

#endif // SUNMOON_H
