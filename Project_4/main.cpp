/**
   Simple cube mapping demo.
   Contains a static cube and an environment mapped object.
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "InputState.h"
#include "Viewer.h"
#include "Table.h"
#include "Skybox.h"
#include "Sphere.hpp"
#include "HeightMap.h"
#include "Shadow.h"
#include "Shader.hpp"
#include "SunMoon.h"
#include "shader.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#define VALS_PER_VERT 3
#define CUBE_NUM_TRIS 12      // number of triangles in a cube (2 per face)
#define CUBE_NUM_VERTICES 8     // number of vertices in a cube`

#define VALS_PER_NORMAL 3
#define VALS_PER_TEX 2
#define SQUARE_NUM_TRIS 2      // number of triangles in a square (2 per face)
#define SQUARE_NUM_VERTICES 4     // number of vertices in a square`


#define LIGHT_SPEED 0.1f

float CAM_SPEED = 0.2f;
int lightSpeedToggle = 0;

int winX = 500;
int winY = 500;

//Lights and shadow attribute/////////////////////////
//float lightX = 0.0f; //orig
//float lightY = 2.0f; // orig
//float lightZ = -0.0f; //orig

float lightX = 0.0f;
float lightY = 5.0f;
float lightZ = -0.0f;

float positionSpheres = -10.0f;
float rotationSpheres = 0.0f;

// This program makes use of the Sphere class, that constructs a sphere mesh.
Sphere *sphere;
unsigned int vaoHandlers[3];
unsigned int texSz = 1024*4;

// Frame buffer object handles
GLuint FramebufferName, depthTexture, depth_rb;
///////////////////////////////////////////////////////////
static float xRot = 0.0f;
static float yRot = 0.0f;
static float camZ = 5.0f;

//Program IDs
unsigned int cubeID;
unsigned int cubeDarkID;
unsigned int reflectID;
unsigned int lightID;
unsigned int tableTestID;
unsigned int sunMoonID;
unsigned int HeightMapID;



unsigned int cubeVaoHandle;
unsigned int* cubeVaoHandleTexMult;
unsigned int cubeVaoHandleDark;
unsigned int tableVaoHandle;
unsigned int cubeTexHandle;
unsigned int cubeTexHandleDark;

float tableWidth = 4.0f;
float tableLength = 3.0f;

//Testing Feature
Table *TheTable;

//Main Features
Skybox *skybox;
Skybox *skyboxDark;
Shadow* shadow;
SunMoon* sun;
SunMoon* moon;

/**
 Cameras
*/
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

int daySwitch =1;
bool  swithing=false;
glm::vec3 cameraPos(xPos, yPos, zPos);

// Data structure storing mouse input info
InputState Input;


using namespace std;

bool ViewReflector = false;

//Height Map
HeightMap *Terrain;


// Set the projection matrix. Takes into account window aspect ratio, so called
// when the window is resized.
void setProjection()
{
    glm::mat4 projection;

    projection = glm::perspective( (float)M_PI/4.0f, (float) winX / winY, 0.5f, 300.0f * camZ );

	// Load it to the shader program
	int projHandle = glGetUniformLocation(cubeID, "projection");
	//int projHandleDark = glGetUniformLocation(cubeDarkID, "projection");
	int projHandleR = glGetUniformLocation(reflectID, "projection");
    int projHandleT = glGetUniformLocation(HeightMapID, "projection");
    int projHandleL = glGetUniformLocation(lightID, "projection");
    //int projHandleSM = glGetUniformLocation(sunMoonID, "projection_matrix");
	if (projHandle == -1 || projHandleR == -1 || projHandleT == -1 || projHandleL == -1 ) {
		std::cout << "Uniform: projection is not an active uniform label\n";
		exit(1);
	}
    glUseProgram(cubeID);
	glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
	//glUseProgram(cubeDarkID);
	//glUniformMatrix4fv( projHandleDark, 1, false, glm::value_ptr(projection) );
    glUseProgram(reflectID);
	glUniformMatrix4fv( projHandleR, 1, false, glm::value_ptr(projection) );
	glUseProgram(HeightMapID);
	glUniformMatrix4fv( projHandleT, 1, false, glm::value_ptr(projection) );
	glUseProgram(lightID);
	glUniformMatrix4fv( projHandleL, 1, false, glm::value_ptr(projection) );
	//glUseProgram(sunMoonID);
	//glUniformMatrix4fv( projHandleSM, 1, false, glm::value_ptr(projection) );
}


// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y )
{
    winX = x;
    winY = y;
    setProjection();
    glViewport( 0, 0, x, y );
}


void render(double dt,Shader shader, Shader hdrShader)
{
    // Update the camera, and draw the scene.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;

    // Draw a big cube surrounding us, textured with cube map
    glUseProgram(cubeID);

    // Use the arrow keys to look around
    Camera->update( Input );

    // First load the viewing matrix from the current camera
    viewMatrix = Camera->getViewMtx();

	// Load it to the shader program
	int viewHandle = glGetUniformLocation(cubeID, "view");
	//int viewHandleDark = glGetUniformLocation(cubeDarkID, "view");
	int viewHandleT = glGetUniformLocation(HeightMapID, "view");
	int viewHandleSM = glGetUniformLocation(sunMoonID, "view");
	//int viewHandleL = glGetUniformLocation(lightID, "view");
	if (viewHandle == -1) {

		std::cout << "Uniform: view is not an active uniform label\n";
	}



	glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMatrix) );
	glUseProgram(HeightMapID);
    glUniformMatrix4fv( viewHandleT, 1, false, glm::value_ptr(viewMatrix) );
    glUseProgram(sunMoonID);
    glUniformMatrix4fv( viewHandleSM, 1, false, glm::value_ptr(viewMatrix) );
    //glUseProgram(lightID);
    //glUniformMatrix4fv( viewHandleL, 1, false, glm::value_ptr(viewMatrix) );

    // fog
    glUseProgram(lightID);
    int fogPos = glGetUniformLocation(lightID, "fogPos");
    if (fogPos == -1) {
		std::cout << "Uniform: fogPos is not an active uniform label\n";
		exit(1);
	}
    glUniform3f(fogPos, Camera->currentX, Camera->currentY, Camera->currentZ);
    // fog end

    int alphaHandle = glGetUniformLocation(cubeID, "alpha");
    int alphaHandleDark = glGetUniformLocation(cubeID, "alphaDark");
    //int alphaDarkHandle = glGetUniformLocation(cubeDarkID, "alpha");
	if (alphaHandle == -1 || alphaHandleDark==-1) {
        std::cerr << "Could not find alpha uniforms\n";
		exit(1);
    }
    glActiveTexture(GL_TEXTURE0);
    sun->renderTwo(Camera,shader,hdrShader,camZ,lightX,lightY,lightZ);
    //moon->renderTwo(Camera,shader,hdrShader,camZ,lightX,lightY,lightZ);
    glUseProgram(cubeID);

    skybox->renderSkybox(cubeID,xPos,yPos,zPos,cubeVaoHandle);
    //Transition day to night
    int deg= (int)dt%360;
    lightZ=cos(dt)*300;
    lightY=sin(dt)*250;
    //cout<<dt<<endl;
    if(lightY<0 && daySwitch!=2){
        cout<<"SWITCH TO NIGHT"<<endl;
        sun->woodTexture=sun->loadTexture("Moon.jpg");
        sun->setup_Texture();
        sun->sunOrMoon=1;
        daySwitch=2;
        //skybox->createCubeMap(daySwitch);
        //glUniform1f(alphaHandle,(1-abs(cos(dt/4))));
    }
    else if(lightY>0 && daySwitch!=1){
        cout<<"SWITCH TO DAY"<<endl;
        sun->woodTexture=sun->loadTexture("sun.jpg");
        sun->setup_Texture();
        sun->sunOrMoon=0;
        daySwitch=1;
        //sun->woodTexture=sun->loadTexture("sun.jpg");
        //skybox->createCubeMap(daySwitch);
    }
    //cout<<"D: "<< abs(cos(dt/2))<<endl;
    //cout<<"N: "<< abs(sin(dt/2))<<endl;
    //daytime
    if(lightY>0){
        //cout<<dt<<endl;
        glUniform1f(alphaHandleDark, abs(sin(dt)));
        //glUniform1f(alphaHandleDark, (float)abs(lightY/250));
        glUniform1f(alphaHandle, abs(cos(dt)));
        //glUniform1f(alphaHandle, (float)abs(lightZ/300));
    }
    //Night time
    else{
        //cout<<"darkness"<<endl;
        glUniform1f(alphaHandle, abs(cos(dt)));
        //glUniform1f(alphaHandle, (float)abs(lightZ/300));
        glUniform1f(alphaHandleDark,(abs(sin(dt))));
        //glUniform1f(alphaHandleDark,((float)abs(lightY/250)));
    }


    //glDisable( GL_BLEND );
    //Terrain->render(HeightMapID);



    glUseProgram(lightID);
    /***************************
     // FBO render pass
     ****************************/
    glBindFramebuffer(GL_FRAMEBUFFER, shadow->FramebufferName);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,shadow->texSz,shadow->texSz);
    shadow->drawScene(1,lightID,Terrain,Camera,camZ);

    /***************************
     // Framebuffer render pass
     ****************************/
    // binds the shadow map for reading
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,winX,winY);
    glBindTexture( GL_TEXTURE_2D, shadow->depthTexture );
    shadow->drawScene(0,lightID,Terrain,Camera,camZ);

    shadow->setLightZ(lightZ);
    shadow->setLightY(lightY);



    glBindVertexArray(0);
    glFlush();

}



//
// Callbacks
//
void key_callback(GLFWwindow* window,
                  int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch(key)
            {
            case GLFW_KEY_ESCAPE: // escape key pressed
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_R:
                ViewReflector = !ViewReflector;
                break;
            //Changes the speed of the camera
            case GLFW_KEY_T:
                lightSpeedToggle=(lightSpeedToggle+1)%2;
                if(lightSpeedToggle==0){
                    CAM_SPEED=0.2f;
                }else{
                    CAM_SPEED=0.7f;
                }
                cout<<"Camera speed toggle mode: "<<lightSpeedToggle<<endl;
                break;
            default:
                break;
            }
    }
    if(key==GLFW_KEY_W){
        lightZ += LIGHT_SPEED;
        shadow->setLightZ(lightZ);
    }
    if(key==GLFW_KEY_A){
        lightX -= LIGHT_SPEED;
        shadow->setLightX(lightX);
    }
    if(key==GLFW_KEY_S){
        lightZ -= LIGHT_SPEED;
        shadow->setLightZ(lightZ);
    }
    if(key==GLFW_KEY_D){
        lightX += LIGHT_SPEED;
        shadow->setLightX(lightX);
    }
    //Move the camera backwards based on the current angle
    if(key==GLFW_KEY_UP){
        zAngle=zAngle-0.2f;
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
    //Move the camera backwards based on the current angle
    else if(key==GLFW_KEY_DOWN){
        zAngle=zAngle+0.2f;
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
    //Move the camera to the left
    else if(key==GLFW_KEY_LEFT){
        xAngle=xAngle-0.2f;
        float deg = Camera->getCurrentDegreeInXAxis()-90;
        if(deg<0){
            deg=360-((int)abs(deg)%360);
        }
        xPos=xPos+cos(deg*M_PI/180);
        zPos=zPos+sin(deg*M_PI/180);
        cout<<"DEG WHEN KEY LEFT: "<<deg<<endl;
        Camera->setCamPos(cos(deg*M_PI/180),0,sin(deg*M_PI/180));
        Camera->setLookAt(cos(deg*M_PI/180),0,sin(deg*M_PI/180));
    }
    //Move the camera to the right
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


/**
 * Error callback for GLFW. Simply prints error message to stderr.
 */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
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




int main (int argc, char **argv)
{

    //skyboxDark=new Skybox();
    shadow = new Shadow(winX,winY,lightX,lightY,lightZ);
    //sphere = new Sphere(1.0f, 16, 16);
    //cout<<sphere->vertCount<<endl;
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(1);
    }
    //sun = new SunMoon();


    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window and OpenGL context
    window = glfwCreateWindow(winX, winY, "Cube mapping", NULL, NULL);
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

	// Set up the shaders we are to use. 0 indicates error.
	cubeID = LoadShaders("cubemap.vert", "cubemap.frag");
	//cubeDarkID = LoadShaders("cubemap.vert", "cubemap.frag");
	reflectID = LoadShaders("reflect.vert", "reflect.frag");
	lightID = LoadShaders("pf-light.vert","pf-light.frag");
	tableTestID = LoadShaders("mview.vert","mview.frag");
	//tableTestID = LoadShaders("pf-light.vert","pf-light.frag");
	//sunMoonID = LoadShaders("sunMoon.vert","sunMoon.frag");
	HeightMapID = LoadShaders("HeightMap/heightMap.vert", "HeightMap/heightMap.frag");
	if (cubeID == 0 || reflectID == 0 || lightID ==0 || HeightMapID ==0 ) {
        fprintf(stderr, "Error loading shaders\n");
		exit(1);
    }

    //Creates Terrain
    Terrain = new HeightMap(lightID);
    if (shadow->setup(lightID,winX,winY,camZ) !=0 ) {
        fprintf(stderr, "Can't initialise shaders here!\n");
        exit(1);
    }

    // Set OpenGL   state we need for this application.
    glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
	glEnable(GL_DEPTH_TEST);

    Shader shader("lighting.vs", "lighting.frag");
    Shader hdrShader("hdr.vs", "hdr.frag");
    sun = new SunMoon(winX,winY,"sun.jpg",0); //Initialization
    sun->setup_Texture();

    skybox = new Skybox();

    // Set up the scene and the cameras
    cubeVaoHandle = skybox->createCubeVAO();
    cubeVaoHandleTexMult = skybox->createMultCubeMap(cubeID);
    //cubeTexHandle = skybox->createCubeMap(daySwitch);

    setProjection();

    WorldCam = new WorldObjectViewer( cameraPos );
    ObjCam = new ObjectViewer( cameraPos );
    Camera = ObjCam;
    Camera->init(xPos,yPos,zPos);



    // Define callback functions and start main loop
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    cout<<"SHADER OPENING FOR SUN"<<endl;
    /**
    *   For sun/moon:
    *   1.Setup shader
    *   2.Initialize light for the sphere (sun/moon)
        3.Set textures
        4.Render
    **/


    //moon = new SunMoon(winX,winY,"Moon.jpg",1);
    //moon->setup_Texture();


    double start = glfwGetTime();
    double now;
    while (!glfwWindowShouldClose(window))
    {
        now = glfwGetTime();

        render( now - start ,shader,hdrShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);

    return 0;
}
