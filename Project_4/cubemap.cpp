/**
   Simple cube mapping demo.
   Contains a static cube and an environment mapped object.
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define VALS_PER_VERT 3
#define CUBE_NUM_TRIS 12      // number of triangles in a cube (2 per face)
#define CUBE_NUM_VERTICES 8     // number of vertices in a cube`

int winX = 500;
int winY = 500;

static float xRot = 0.0f;
static float yRot = 0.0f;
static float camZ = 5.0f;

unsigned int cubeID;
unsigned int reflectID;
unsigned int cubeVaoHandle;
unsigned int cubeTexHandle;

bool ViewReflector = false;

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
            default:
                break;
            }
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch(key) {
        case GLFW_KEY_LEFT:
            yRot -= 0.04;
            break;
        case GLFW_KEY_RIGHT:
            yRot += 0.04;
            break;
        case GLFW_KEY_UP:
            xRot += 0.04;
            break;
        case GLFW_KEY_DOWN:
            xRot -= 0.04;
            break;
        }
    }    
    
}	

int createCubeVAO()
{
    // Positions and normals combined in one array
    // Could equally use 2 separate arrays as before 
    GLfloat cubeVertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 

        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,

        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
    };
    
	unsigned int vaoHandle;
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);
  
	// Buffers to store position, colour and index data
	unsigned int buffer[2];
	glGenBuffers(2, buffer);

	// Set vertex position
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

	// Set vertex normal
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),(GLvoid*)(3 * sizeof(GLfloat)));
    
    // Un-bind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    
	return vaoHandle;
}


// Set the projection matrix. Takes into account window aspect ratio, so called
// when the window is resized.
void setProjection()
{
    glm::mat4 projection;
    
    projection = glm::perspective( (float)M_PI/4.0f, (float) winX / winY, 0.5f, 6 * camZ );

	// Load it to the shader program
	int projHandle = glGetUniformLocation(cubeID, "projection");
	int projHandleR = glGetUniformLocation(reflectID, "projection");

	if (projHandle == -1 || projHandleR == -1) {
		std::cout << "Uniform: projection is not an active uniform label\n";
	}
    glUseProgram(cubeID);
	glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
    glUseProgram(reflectID);
	glUniformMatrix4fv( projHandleR, 1, false, glm::value_ptr(projection) );    
}

// Load the 6 textures for the cube map and set parameters
unsigned int createCubeMap()
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    char const *paths[6] = { "skybox/sky_right.jpg", "skybox/sky_left.jpg", \
                       "skybox/sky_top.jpg", "skybox/sky_bottom.jpg",   \
                       "skybox/sky_back.jpg", "skybox/sky_front.jpg" };
    GLuint faces[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                       GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                       GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                       GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                       GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                       GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
    
    int x, y, n;
    unsigned char *data;
    
    for (int i = 0; i < 6; i++) {
        
        data = stbi_load( paths[i], &x, &y, &n, 3 ); // force RGB format        

        glTexImage2D(faces[i], 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        
        stbi_image_free(data);
    }
       
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);      

    return textureID;
}

// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y ) 
{
    winX = x;
    winY = y;
    setProjection();
    glViewport( 0, 0, x, y );
}

void render() 
{
    // Update the camera, and draw the scene.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;

    // Draw a big cube surrounding us, textured with cube map
    glUseProgram(cubeID);
    
	int viewHandle = glGetUniformLocation(cubeID, "view");
    int modelHandle = glGetUniformLocation(cubeID, "model");
	if (viewHandle == -1 || modelHandle == -1) {
		std::cout << "Uniform: model or view is not an active uniform label\n";
	}

    // Use the arrow keys to look around
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -camZ));
    viewMatrix = glm::rotate(viewMatrix, yRot,
                             glm::vec3(0.0f, 1.0f, 0.0f));
    viewMatrix = glm::rotate(viewMatrix, xRot,
                             glm::vec3(1.0f, 0.0f, 0.0f));   

    // Scale the cube to be outside the camera 
    modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
        
    glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMatrix) );
    glUniformMatrix4fv( modelHandle, 1, false, glm::value_ptr(modelMatrix) );

    // Draw the cube
    glBindVertexArray(cubeVaoHandle);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw a smaller cube using reflection mapping
    if (ViewReflector) {
        
        glUseProgram(reflectID);

        viewHandle = glGetUniformLocation(reflectID, "view");
        if (viewHandle == -1) {
            std::cout << "Uniform: view is not an active uniform label\n";
        }
        glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMatrix) );

        glBindVertexArray(cubeVaoHandle);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }    
    glBindVertexArray(0);
    glFlush();

}

/**
 * Error callback for GLFW. Simply prints error message to stderr.
 */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

int main (int argc, char **argv)
{
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
	reflectID = LoadShaders("reflect.vert", "reflect.frag");
	if (cubeID == 0 || reflectID == 0) {
        fprintf(stderr, "Error loading shaders\n");
		exit(1);
    }

    // Set OpenGL state we need for this application.
    glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
	glEnable(GL_DEPTH_TEST);
    
    // Set up the scene and the cameras
    cubeVaoHandle = createCubeVAO();
    cubeTexHandle = createCubeMap();
    setProjection();
    
    // Define callback functions and start main loop
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);

    while (!glfwWindowShouldClose(window))
    {
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);
    
    return 0;
}
