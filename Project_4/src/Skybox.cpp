#include "Skybox.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"


#define VALS_PER_VERT 3
#define CUBE_NUM_TRIS 12      // number of triangles in a cube (2 per face)
#define CUBE_NUM_VERTICES 8     // number of vertices in a cube`

Skybox::Skybox()
{
    //ctor
}

// Load the 6 textures for the cube map and set parameters


unsigned int* Skybox::createMultCubeMap(unsigned int programID){
    GLuint textureID[2];
    glGenTextures(2, textureID);

    //DAY TIME
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID[0]);

    char const *paths[6];

    paths[0]="skybox/6right.png";paths[1]="skybox/6left.png";
    paths[2]="skybox/6top.png";paths[3]="skybox/6bottom.png";
    paths[4]="skybox/6back.png";paths[5]="skybox/6front.png";
    //paths[0]="skybox/debugRight2.png";paths[1]="skybox/debugLeft2.png";
    //paths[2]="skybox/debugTop2.png";paths[3]="skybox/debugBottom2.png";
    //paths[4]="skybox/debugBack2.png";paths[5]="skybox/debugFront2.png";
//    paths[0]="skybox/darkRight.png";paths[1]="skybox/darkLeft.png";
//    paths[2]="skybox/darkTopDebug4.png";paths[3]="skybox/darkBottom.png";
//    paths[4]="skybox/darkBack.png";paths[5]="skybox/darkFront.png";

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
    int hand = glGetUniformLocation(programID, "dayBox");
    if(hand==-1){
        cout<<"NOPE"<<endl;
        exit(1);
    }
    glUniform1i(glGetUniformLocation(programID, "dayBox"), 0);

    //NIGHT TIME
    //glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID[1]);

    char const *pathsDark[6];

    pathsDark[0]="skybox/darkRight.png";pathsDark[1]="skybox/darkLeft.png";
    pathsDark[2]="skybox/darkTop.png";pathsDark[3]="skybox/darkBottom.png";
    pathsDark[4]="skybox/darkBack.png";pathsDark[5]="skybox/darkFront.png";

    GLuint facesDark[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                       GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                       GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                       GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                       GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                       GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

    unsigned char *dataDark;

    for (int i = 0; i < 6; i++) {

        dataDark = stbi_load( pathsDark[i], &x, &y, &n, 3 ); // force RGB format

        glTexImage2D(facesDark[i], 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, dataDark);

        stbi_image_free(dataDark);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    hand = glGetUniformLocation(programID, "nightBox");
    if(hand==-1){
        cout<<"NOPE"<<endl;
        exit(1);
    }

    glUniform1i(glGetUniformLocation(programID, "nightBox"), 1);
    textures[0]=textureID[0];
    textures[1]=textureID[1];
    return textureID;

}
int Skybox::createCubeVAO()
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

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

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

	return vao;
}

void Skybox::renderSkybox(int cubeID,float xPos, float yPos, float zPos, int cubeVaoHandle){

    glm::mat4 modelMatrix;
    glUseProgram(cubeID);
//    glActiveTexture(GL_TEXTURE1);
//    glEnable(GL_TEXTURE_CUBE_MAP);
//    glBindTexture(GL_TEXTURE_CUBE_MAP,textures[1]);
    int modelHandle = glGetUniformLocation(cubeID, "model");
	if (modelHandle == -1) {
		std::cout << "Uniform: model or view is not an active uniform label\n";
	}
	int todHandle= glGetUniformLocation(cubeID, "tod");
    if(todHandle==-1){
        std::cerr << "Could not find tod uniform\n";
		exit(1);
    }

    // Scale the cube to be outside the camera
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));//original
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(2.5f, 2.5f, 2.5f));
    modelMatrix = glm::translate(modelMatrix,glm::vec3(xPos,yPos,zPos));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(400.0f, 400.0f, 400.0f));

    glUniformMatrix4fv( modelHandle, 1, false, glm::value_ptr(modelMatrix) );

    // Draw the cube
    glBindVertexArray(cubeVaoHandle);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
   //glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glBindTexture(GL_TEXTURE_CUBE_MAP,textures[0]);
    glUniform1i(todHandle,0);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindTexture(GL_TEXTURE_CUBE_MAP,textures[1]);
    glUniform1i(todHandle,1);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}




Skybox::~Skybox()
{
    //dtor
}
