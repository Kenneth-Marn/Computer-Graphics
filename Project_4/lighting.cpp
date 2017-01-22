/*********************************************************
 Version 1.2
 
 Code provided by Michael Hemsley and Anthony Dick
 for the course
 COMP SCI 3014/7090 Computer Graphics
 School of Computer Science
 University of Adelaide
 
 Permission is granted for anyone to copy, use, modify, or distribute this
 program and accompanying programs and documents for any purpose, provided
 this copyright notice is retained and prominently displayed, along with
 a note saying that the original programs are available from the aforementioned
 course web page.
 
 The programs and documents are distributed without any warranty, express or
 implied.  As the programs were written for research purposes only, they have
 not been tested to the degree that would be advisable in any important
 application.  All use of these programs is entirely at the user's own risk.
 *********************************************************/

/**
 * Demonstrates use of texture to supply diffuse and specular colour
 * for a surface.
 *
 * Textures from NASA visible earth: http://visibleearth.nasa.gov
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "Sphere.hpp"
using namespace std;
#include "vector"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Viewer.h"
#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_TEX 2
#define LIGHT_SPEED 0.2f
#define NUM_TEXTURES 2

// Window width and height
unsigned int winW = 640;
unsigned int winH = 480;

Sphere *sphere;
unsigned int sphereVaoHandle;
GLuint hdrFBO;
GLuint colorBuffer;
GLuint rboDepth;
GLuint woodTexture;
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;


WorldObjectViewer *WorldCam;
ObjectViewer *ObjCam;
Viewer *Camera;

//glm::vec3 cameraPos(0.0f, 5.0f, -12.0f); //The original
float xPos=0.0f;
float yPos=0.0f;
float zPos=0.0f;
float xAngle= xPos;
float yAngle= yPos;
float zAngle= -0.0f;
float CAM_SPEED = 0.5f;
int daySwitch =0;
bool  swithing=false;
glm::vec3 cameraPos(xPos, yPos, zPos);

// Data structure storing mouse input info
InputState Input;

// Camera
//glm::vec3 cameraPos(3.0f, 2.0f, 15.0f);

std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;

// Delta
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Options
GLboolean hdr = true; // Change with 'Space'
GLfloat exposure = 20.0f; // Change with Q and E


/**
 * Creates a new vertex array object and loads sphere data onto GPU.
 */

int createSphereVAO()
{
    // Using the sphere class to generate vertices and element indices
    sphere = new Sphere(1.0f, 16, 16);
    
    glGenVertexArrays(1, &sphereVaoHandle);
    glBindVertexArray(sphereVaoHandle);
    
    unsigned int buffer[4];
    glGenBuffers(4, buffer);
    
    // Set vertex position attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->vertCount,
                 sphere->vertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
    
    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->normCount,
                 sphere->normals,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);
    
    // Tex coords
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->texCount,
                 sphere->texcoords,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);
    
    // Vertex indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * sphere->indCount,
                 sphere->indices,
                 GL_STATIC_DRAW);
    
    return 0; // success
}



// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 36);
    glBindVertexArray(0);
    
}

void RenderCube()
{
     createSphereVAO();
     glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);
}
GLuint loadTexture(GLchar const * path)
{
    
    
    lightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f)); // back light
    lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
    lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
    lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
    
    
    
    lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
    lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
    lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
    lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));
    // Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image = stbi_load(path, &width, &height, 0, 3);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);
    return textureID;
    
}
void setup_Texture()
{
    glGenFramebuffers(1, &hdrFBO);
    // - Create floating point color buffer
    
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, winW, winH, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // - Create depth buffer (renderbuffer)
    
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, winW, winH);
    // - Attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
}
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}
void render(Shader hdrShader,Shader shader)
{
    // Use the arrow keys to look around
    Camera->update( Input );
    
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 projection = glm::perspective(M_PI/4.0, double(winW) / double(winH), 1.0, 30.0);
    glm::mat4 view       = Camera->getViewMtx();
    
    glm::mat4 model;
    
    shader.Use();
    int proHandle=glGetUniformLocation(shader.Program, "projection");
    int viewhandle=glGetUniformLocation(shader.Program, "view");
    glUniformMatrix4fv(proHandle, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewhandle,1, GL_FALSE, glm::value_ptr(view));
    if (proHandle == -1 || viewhandle==-1 ) {
        fprintf(stderr, "Error: can't find texture maps\n");
        exit(1);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    // - set lighting uniforms
    for (GLuint i = 0; i < lightPositions.size(); i++)
    {
        glUniform3fv(glGetUniformLocation(shader.Program, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &lightPositions[i][0]);
        glUniform3fv(glGetUniformLocation(shader.Program, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &lightColors[i][0]);
    }
        // - render tunnel
    
    
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0));
    model = glm::scale(model, glm::vec3(5.0f, 5.0f, 50.0f));
    
    
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader.Program, "inverse_normals"), GL_TRUE);
    
    RenderCube();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // 2. Now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    hdrShader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    
    glUniform1i(glGetUniformLocation(hdrShader.Program, "hdr"), hdr);
    glUniform1f(glGetUniformLocation(hdrShader.Program, "exposure"), exposure);
    RenderQuad();
    glFlush();
}


//callbacks
void key_callback(GLFWwindow* window,
                  int key, int scancode, int action, int mods)
{
    
    
    if(key==GLFW_KEY_UP){
        zAngle=zAngle-0.2f;
        //Camera->setCamPos(0,0,-0.2f);
        //Camera->setLookAt(0.0f,0,-0.2f);
        xPos=xPos+cos(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED;
        yPos=yPos+sin(Camera->getCurrentDegreeInYAxis()*M_PI/180)*CAM_SPEED;
        zPos=zPos+sin(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED;
        Camera->setCamPos(cos(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED
                          ,sin(Camera->getCurrentDegreeInYAxis()*M_PI/180)*CAM_SPEED
                          ,sin(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED);
        Camera->setLookAt(cos(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED
                          ,sin(Camera->getCurrentDegreeInYAxis()*M_PI/180)*CAM_SPEED
                          ,sin(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED);
    }
    else if(key==GLFW_KEY_DOWN){
        zAngle=zAngle+0.2f;
        //Camera->setCamPos(0,0,0.2f);
        //Camera->setLookAt(0.0f,0,0.2f);
        xPos=xPos-cos(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED;
        yPos=yPos-sin(Camera->getCurrentDegreeInYAxis()*M_PI/180)*CAM_SPEED;
        zPos=zPos-sin(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED;
        Camera->setCamPos(-cos(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED
                          ,-sin(Camera->getCurrentDegreeInYAxis()*M_PI/180)*CAM_SPEED
                          ,-sin(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED);
        Camera->setLookAt(-cos(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED
                          ,-sin(Camera->getCurrentDegreeInYAxis()*M_PI/180)*CAM_SPEED
                          ,-sin(Camera->getCurrentDegreeInXAxis()*M_PI/180)*CAM_SPEED);
    }
    else if(key==GLFW_KEY_LEFT){
        xAngle=xAngle-0.2f;
        float deg = Camera->getCurrentDegreeInXAxis()-90;
        if(deg<0){
            deg=360-((int)abs(deg)%360);
        }
        xPos=xPos+cos(deg*M_PI/180);
        zPos=zPos+sin(deg*M_PI/180);
        cout<<"DEG WHEN KEY LEFT: "<<deg<<endl;
        //Camera->setCamPos(-0.2f,0,0.0f);
        //Camera->setLookAt(-0.2f,0,0);
        Camera->setCamPos(cos(deg*M_PI/180),0,sin(deg*M_PI/180));
        Camera->setLookAt(cos(deg*M_PI/180),0,sin(deg*M_PI/180));
    }
    else if(key==GLFW_KEY_RIGHT){
        xAngle=xAngle+0.2f;
        float deg = Camera->getCurrentDegreeInXAxis()+90;
        if(deg<0){
            deg=(int)deg%360;
        }
        xPos=xPos+cos(deg*M_PI/180);
        zPos=zPos+sin(deg*M_PI/180);
        //Camera->setCamPos(0.2f,0,0.0f);
        //Camera->setLookAt(0.2f,0,0);
        Camera->setCamPos(cos(deg*M_PI/180),0,sin(deg*M_PI/180));
        Camera->setLookAt(cos(deg*M_PI/180),0,sin(deg*M_PI/180));
    }
    
}


// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y )
{
    winW = x;
    winH = y;
    //setProjection();
    glViewport( 0, 0, x, y );
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        Input.rMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        Input.rMousePressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        Input.lMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Input.lMousePressed = false;
    }
}


void mouse_pos_callback(GLFWwindow* window, double x, double y)
{
    // Use a global data structure to store mouse info
    // We can then use it however we want
    Input.update((float)x, (float)y);
}

/**
 * Program entry.
 */

int main(int argc, char **argv)
{
    printf("Controls: arrow keys - light movement.\n");
    
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        exit(1);
    }
    
    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create the window and OpenGL context
    window = glfwCreateWindow(winW, winH, "Lighting", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(1);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
    
    Shader shader("lighting.vs", "lighting.frag");
    Shader hdrShader("hdr.vs", "hdr.frag");

    
    // Load textures
    woodTexture = loadTexture("Moon.jpg");
    setup_Texture();
    WorldCam = new WorldObjectViewer( cameraPos );
    ObjCam = new ObjectViewer( cameraPos );
    Camera = ObjCam;

    // Define callback functions and start main loop
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        render(hdrShader,shader);
       
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
