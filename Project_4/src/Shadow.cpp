#include "Shadow.h"
#define VALS_PER_VERT 3
#define CUBE_NUM_TRIS 12      // number of triangles in a cube (2 per face)
#define CUBE_NUM_VERTICES 8     // number of vertices in a cube`

#define VALS_PER_NORMAL 3
#define VALS_PER_TEX 2
#define SQUARE_NUM_TRIS 2      // number of triangles in a square (2 per face)
#define SQUARE_NUM_VERTICES 4     // number of vertices in a square`



#define LIGHT_SPEED 0.1f

Shadow::Shadow()
{
    lightX = 0.0f;
    lightY = 5.0f;
    lightZ = -0.0f;

    positionSpheres = -10.0f;
    rotationSpheres = 0.0f;
    //ctor
}

Shadow::Shadow(int sizex, int sizey){
    winX = 500;
    winY = 500;

    lightX = 0.0f;
    lightY = 2.0f;
    lightZ = -0.0f;

    positionSpheres = -10.0f;
    rotationSpheres = 0.0f;

}
Shadow::Shadow(int,int,float xlight,float ylight,float zlight){
    winX = 500;
    winY = 500;

    lightX = xlight;
    lightY = ylight;
    lightZ = zlight;

    positionSpheres = -10.0f;
    rotationSpheres = 0.0f;

}
/**
 * Creates a new vertex array object and loads square data onto GPU.
 */
int Shadow::createSquareVAO()
{
    // square has 4 vertices at its corners
	float squareVertices[ SQUARE_NUM_VERTICES*VALS_PER_VERT ] = {
        -1.0f, 0.0f, -1.0f,
        1.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, 1.0f
	};

    // Normals of each vertex
    float squareNormals[ SQUARE_NUM_VERTICES*VALS_PER_VERT ] = {
        0.0f, 1.0f, 0.0f ,
        0.0f, 1.0f, 0.0f ,
        0.0f, 1.0f, 0.0f ,
        0.0f, 1.0f, 0.0f
	};

    // Texture indices of each vertex
    float squareTexture[ SQUARE_NUM_VERTICES*VALS_PER_TEX ] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
	};

    // Each square face is made up of two triangles
    unsigned int indices[SQUARE_NUM_TRIS * 3] = {
        0,1,2, 2,3,0
    };

	// Buffers to store position, colour, normal and index data
	unsigned int buffer[4];
    glGenBuffers(4, buffer);

	// Set vertex position
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices), indices, GL_STATIC_DRAW);

    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(squareNormals), squareNormals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Texture attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(squareTexture), squareTexture, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

	return 1;
}

/**
 * Creates a new vertex array object and loads sphere data onto GPU.
 */
int Shadow::createSphereVAO()
{
    if (!sphere) {
        fprintf(stderr, "Error: initalise sphere before creating VAO!\n");
    }

	// Buffers to store position, colour, normal and index data
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

	// Vertex indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * sphere->indCount,
                 sphere->indices,
                 GL_STATIC_DRAW);

    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->normCount,
                 sphere->normals,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

    //Texture
    glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*sphere->texCount, sphere->texcoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

	return 1;
}

/**
 * Sets the shader uniform variables and create vertex data
 * @return success on 0, failure otherwise (From Shadow Example)
 */
int Shadow::setup(unsigned int ProgramID,int& winX, int& winY, float camZ)
{
	// Call the reshape function explicitly on setup
	reshape(winX, winY, ProgramID,camZ);

    glUseProgram(ProgramID);

    // Generate the 2 VAOs
    glGenVertexArrays(3, vaoHandlers);

	// Using the sphere class to generate vertices and element indices
	sphere = new Sphere(1.0f, 16, 16);
	glBindVertexArray(vaoHandlers[0]);
    if(createSphereVAO()!=1){
        cout<<"FAIL"<<endl;
        exit(1);
    };

    // create square
    glBindVertexArray(vaoHandlers[1]);
    if(createSquareVAO()!=1){
        cout<<"FAIL"<<endl;
        exit(1);
    };

    //glBindVertexArray(vaoHandlers[2]);
    //box->createCubeVAO();

    // Un-bind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Uniform lighting variables
    int lightambientHandle = glGetUniformLocation(ProgramID, "light_ambient");
	int lightdiffuseHandle = glGetUniformLocation(ProgramID, "light_diffuse");
	int lightspecularHandle = glGetUniformLocation(ProgramID, "light_specular");
	if ( lightambientHandle == -1 ||
         lightdiffuseHandle == -1 ||
         lightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find light uniform variables\n");
		return 1;
    }

	float lightambient[3] = { 0.5f, 0.5f, 0.5f };	// ambient light components
    float lightdiffuse[3] = { 1.0f, 1.0f, 1.0f };	// diffuse light components
    float lightspecular[3] = { 1.0f, 1.0f, 1.0f };	// specular light components

	glUniform3fv(lightambientHandle, 1, lightambient);
	glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
	glUniform3fv(lightspecularHandle, 1, lightspecular);

    // generate handles for the frame buffer and depth buffer it contains
    glGenFramebuffers(1, &FramebufferName);
    glGenTextures(1, &depthTexture);

    //switch to our fbo so we can bind stuff to it
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // create the depth texture and attach it to the frame buffer.
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D,
                 0, GL_DEPTH_COMPONENT, texSz, texSz,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "depthTexture" as our depth attachement
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           depthTexture, 0);

    // Instruct openGL that we won't bind a color texture with the current FBO
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return 1;
    }
    cout<<"SETUP LIGHT FIN"<<endl;
	return 0;	// return success
}

/*
 This function draws the models for both shaders
 */
void Shadow::drawScene(int shadow, int ProgramID, HeightMap *Terrain,Viewer *Camera,float camZ)
{
    int projHandle = glGetUniformLocation(ProgramID, "projection" );
    int mvHandle = glGetUniformLocation(ProgramID, "model");
	int normHandle = glGetUniformLocation(ProgramID, "normal_matrix");
	int lightposHandle = glGetUniformLocation(ProgramID, "light_pos");
    int depthBiasMVPHandle = glGetUniformLocation(ProgramID, "DepthBiasMVP");
    int shadowHandle = glGetUniformLocation(ProgramID, "shadow");

	if (projHandle == -1 || mvHandle == -1 || normHandle==-1 || lightposHandle == -1 || depthBiasMVPHandle == -1) {
        fprintf(stderr, "Error: can't find matrix uniforms in drawscene function\n");
        cout<<projHandle<<","<<mvHandle<<","<<normHandle<<","<<lightposHandle<<","<<depthBiasMVPHandle<<endl;
		exit(1);
    }

    // set the shadow variable in frag shader
    glUniform1i(shadowHandle, shadow);

    // Update the light direction
	float lightPos[4] = { lightX, lightY, lightZ, 0.0f };
    glUniform4fv(lightposHandle, 1, lightPos);

    // Uniform variables defining material colours
    // These can be changed for each sphere, to compare effects
    int mtlambientHandle = glGetUniformLocation(ProgramID, "mtl_ambient");
	int mtldiffuseHandle = glGetUniformLocation(ProgramID, "mtl_diffuse");
	int mtlspecularHandle = glGetUniformLocation(ProgramID, "mtl_specular");
	if ( mtlambientHandle == -1 ||
        mtldiffuseHandle == -1 ||
        mtlspecularHandle == -1) {
        fprintf(stderr, "Error: can't find material uniform variables\n");
		exit(1);
    }

	float mtlambient[3] = { 0.3f, 0.3f, 0.3f };	// ambient material
    float mtldiffuse[3] = { 0.5f, 0.5f, 0.5f };	// diffuse material
    float mtlspecular[3] = { 1.0f, 1.0f, 1.0f };	// specular material

	glUniform3fv(mtlambientHandle, 1, mtlambient);
	glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
	glUniform3fv(mtlspecularHandle, 1, mtlspecular);
    //cout<<"PART 1.1"<<endl;

    // viewing matrix
    glm::mat4 viewMatrix = Camera->getViewMtx();
    glm::mat4 projection;
    glm::mat4 depthBiasMVP, projLightMatrix, viewLightMatrix, mvLightMatrix;
    // take values in [-1,1] from proj_model_view in light space and transform
    // them to [0,1] in order to index the texture map (from depth buffer)
    // 0.5 in diag transform points from [-1,1] to [-.5,.5] and last row
    // translate them +0.5
    glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0,
                         0.0, 0.5, 0.0, 0.0,
                         0.0, 0.0, 0.5, 0.0,
                         0.5, 0.5, 0.5, 1.0);

    glm::mat4 mvMatrix;
    glm::mat3 normMatrix;

    if (shadow == 0) {
        // Store the light projection and view matrix in variables
        viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, positionSpheres));
        getPVLightMatrix(&projLightMatrix, &viewLightMatrix);
        reshape(winX, winY, ProgramID,camZ);
    }
    else {
        // Shadow pass
        // Use the light projection and view matrix when rendering
        getPVLightMatrix(&projection, &viewMatrix);
        glUniformMatrix4fv(projHandle, 1, false, glm::value_ptr(projection) );
    }
    //cout<<"PART 1.2"<<endl;

	// Set VAO to the sphere mesh
    glBindVertexArray(vaoHandlers[0]);
    int sphereRenderHandle = glGetUniformLocation(ProgramID,"sphereRender");
    glUniform1i(sphereRenderHandle, 1);
	// Render three spheres in a row
    // On the FBO pass, just draw the spheres and floor from the point of
    // view of the light. The depth values are stored in a texture.
    // On the second render pass, draw the spheres and floor from the
    // point of view of the camera.
    mvMatrix = glm::rotate(viewMatrix, rotationSpheres,
                           glm::vec3(0.0f, 1.0f, 0.0f));
    normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));
	glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );	// Middle
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
    if (shadow == 0) {
        // Set up matrices to compare depths at each fragment
        // DepthBiasMVP is the model-view-projection matrix for the light
        // with a scaling to make it match texture coordinates
        mvLightMatrix = glm::rotate(viewLightMatrix, rotationSpheres, glm::vec3(0.0f, 1.0f, 0.0f));
        depthBiasMVP = biasMatrix * projLightMatrix * mvLightMatrix;
        glUniformMatrix4fv(depthBiasMVPHandle, 1, false, glm::value_ptr(depthBiasMVP) );
    }
	glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);

    mvMatrix = glm::translate(mvMatrix, glm::vec3(-3.0f, 0.0f, 0.0f));
    normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));
	glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );	// Left
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
	if (shadow == 0) {
        mvLightMatrix = glm::translate(mvLightMatrix, glm::vec3(-3.0f, 0.0f, 0.0f));
        depthBiasMVP = biasMatrix * projLightMatrix * mvLightMatrix;
        glUniformMatrix4fv(depthBiasMVPHandle, 1, false, glm::value_ptr(depthBiasMVP) );
    }

    glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);

    mvMatrix = glm::translate(mvMatrix, glm::vec3(6.0f, 0.0f, 0.0f));
    normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));
	glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );	// right
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
	if (shadow == 0) {
        mvLightMatrix = glm::translate(mvLightMatrix, glm::vec3(6.0f, 0.0f, 0.0f));
        depthBiasMVP = biasMatrix * projLightMatrix * mvLightMatrix;
        glUniformMatrix4fv(depthBiasMVPHandle, 1, false, glm::value_ptr(depthBiasMVP) );
    }
    glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);
    glUniform1i(sphereRenderHandle, 0);

    // render floor
    glBindVertexArray(vaoHandlers[1]);

    // floor
    //mvMatrix = glm::translate(viewMatrix, glm::vec3(0.0f,-5.0, 0.0f));
    mvMatrix = glm::translate(viewMatrix, glm::vec3(0.0f,20.0, 0.0f));
    mvMatrix = glm::scale(mvMatrix, glm::vec3(20.0,20.0,20.0));
    normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));
	glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );	// right
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
    if (shadow == 0) {
        mvLightMatrix = glm::translate(viewLightMatrix, glm::vec3(0.0f,-5.0, 0.0f));
        mvLightMatrix = glm::scale(mvLightMatrix, glm::vec3(20.0,20.0,20.0));
        depthBiasMVP = biasMatrix*projLightMatrix*mvLightMatrix;
        glUniformMatrix4fv(depthBiasMVPHandle, 1, false, glm::value_ptr(depthBiasMVP) );
    }
	//glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);


    ///////////////// TEST RENDER CUBE/HEIGHTMAP HERE ///////////////////////
    // render table(or heightmap) (TESTING)
    glUseProgram(ProgramID);

	int modelUniformHandle = glGetUniformLocation(ProgramID, "model");
	if (modelUniformHandle == -1){
		exit(1);
	}
    glBindVertexArray(Terrain->vaoHeightHandle);
    int mapRenderHandle = glGetUniformLocation(ProgramID, "mapRender");
    glUniform1i(mapRenderHandle, 1);
    // cube test
    mvMatrix = glm::mat4(1.0f);
    //mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f,-0.0, -5.0f));
    //mvMatrix = glm::scale(mvMatrix, glm::vec3(1.0,1.0,1.0));
    mvMatrix = glm::translate(viewMatrix, glm::vec3(-13.0f,-5.0, -215.0f));
    mvMatrix = glm::scale(mvMatrix, glm::vec3(250.0, 250.0, 250.0));
    //mvMatrix = glm::scale(mvMatrix, glm::vec3(1.0,1.0,1.0));
    normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));
	glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );	// right
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
    //cout<<"DRAWING TIME"<<endl;
    if (shadow == 0) {
        //mvLightMatrix = glm::translate(viewLightMatrix, glm::vec3(0.0f,-5.0, 0.0f));
        //mvLightMatrix = glm::scale(mvLightMatrix, glm::vec3(20.0,20.0,20.0));
        mvLightMatrix = glm::translate(viewLightMatrix, glm::vec3(-13.0f,-5.0, -215.0f));
        mvLightMatrix = glm::scale(mvLightMatrix, glm::vec3(250.0, 250.0, 250.0));
        depthBiasMVP = biasMatrix*projLightMatrix*mvLightMatrix;
        glUniformMatrix4fv(depthBiasMVPHandle, 1, false, glm::value_ptr(depthBiasMVP) );
    }
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glm::mat4 model;
    //model = glm::scale( model, glm::vec3(250.0, 250.0, 250.0) );
    //model = glm::translate( model, glm::vec3(-Terrain->numeratorW/2, 0.0, -Terrain->numeratorH/2) );

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniformMatrix4fv(  mvHandle, 1, false, glm::value_ptr(mvMatrix) );
    glDrawElements(GL_TRIANGLES, (Terrain->width-1)*(Terrain->height-1)*6, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUniform1i(mapRenderHandle, 0);
}

/*
 Function to get the projection*view light matrix
 */
void Shadow::getPVLightMatrix(glm::mat4 *projection, glm::mat4 *viewMatrix)
{
    glm::vec3 lightInvDir = glm::vec3(lightX, lightY, lightZ);

    //*projection = glm::ortho<float>(-40,40,-40,40,-1,100);
    //*projection = glm::ortho<float>(-260,260,-260,260,-260,260);
    *projection = glm::ortho<float>(-260,260,-260,260,-260,260);

    *viewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
}

void Shadow::reshape(int width, int height,int ProgramID, float camZ)
{
	// Define the size and sub-window position of the render area in pixels
    //	glViewport(0, 0, width, height);

    winY = height;
    winX = width;

	// Perspective projection matrix
    glm::mat4 projection;
    //projection = glm::perspective(M_PI/4, double(winX) / double(winY), 1.0, 50.0 );
    projection = glm::perspective( (float)M_PI/4.0f, (float) winX / winY, 0.5f, 300.0f * camZ );
	// Load it to the shader program
	int projHandle = glGetUniformLocation(ProgramID, "projection");
	if (projHandle == -1) {
        fprintf(stderr, "Error updating proj matrix\n");
        exit(1);
    }
    glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
}

void Shadow::setLightX(float x){
    lightX =x;
}

void Shadow::setLightZ(float z){
    lightZ =z;
}
void Shadow::setLightY(float y){
    lightY =y;
}

Shadow::~Shadow()
{
    //dtor
}
