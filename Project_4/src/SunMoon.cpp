#include "SunMoon.h"

// RenderCube() Renders a 1x1 3D cube in NDC.
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;

GLuint quadVAO = 0;
GLuint quadVBO;

// Delta
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


// Options
GLboolean hdr = true; // Change with 'Space'
GLfloat exposure = 20.0f * 5; // Change with Q and E
//From stackoverflow source because for some reason to_string isn't provided from namespace std
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

SunMoon::SunMoon()
{
    //createTextures();
    int res= createSphereVAO();
    if(res!=0){
        cout<<"Failed to create sun/moon"<<endl;
    }

    //ctor
}

SunMoon::SunMoon(int windowW,int windowH,const char* path,int identifier)
{
    //createTextures();
//    int res= createSphereVAO();
//    if(res!=0){
//        cout<<"Failed to create sun/moon"<<endl;
//    }
    winW=windowW;
    winH = windowH;
    woodTexture=loadTexture(path);
    sunOrMoon = identifier;
    lightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f)); // back light
    lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
    lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
    lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));



    lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
    lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
    lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.2f));
    lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.1f));
    //ctor
}

/**
 * Creates a new vertex array object and loads sphere data onto GPU.
 */
int SunMoon::createSphereVAO()
{
    // Using the sphere class to generate vertices and element indices
    sunMoon = new Sphere(1.0f, 16, 16);
    glGenVertexArrays(1, &sunMoonVaoHandle);
    glBindVertexArray(sunMoonVaoHandle);
    //float *verts = sunMoon->vertices;

    unsigned int buffer[4];
    glGenBuffers(4, buffer);
    // Set vertex position attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sunMoon->vertCount,
                 sunMoon->vertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
    //cout<<"1.1: "<<sunMoon->texCount<<endl;
    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sunMoon->normCount,
                 sunMoon->normals,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);
    //cout<<"1.2"<<endl;
    // Tex coords
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sunMoon->texCount,
                 sunMoon->texcoords,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

    // Vertex indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * sunMoon->indCount,
                 sunMoon->indices,
                 GL_STATIC_DRAW);

    return 0; // success
}




void SunMoon::RenderCube()
{
     createSphereVAO();
//     glBindVertexArray(sunMoonVaoHandle);
//     glm::mat4 model;
//     model = glm::mat4(1.0f);
//     model = glm::translate(model, glm::vec3(1,5,5));
//     model = glm::scale(model, glm::vec3(1.0f, 1.0f, 50.0f));
     glDrawElements(GL_TRIANGLES, sunMoon->indCount, GL_UNSIGNED_INT, 0);
}

void SunMoon::RenderQuad()
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
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

}
GLuint SunMoon::loadTexture(GLchar const * path)
{
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

void SunMoon::renderTwo(Viewer *Camera,Shader shader,Shader hdrShader,float camZ,float xPos, float yPos, float zPos){
    // Use the arrow keys to look around
    //Camera->update( Input );

    // Set frame time
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 projection = glm::perspective(M_PI/4.0, double(winW) / double(winH), 1.0, 300.0*camZ);
    glm::mat4 view       = Camera->getViewMtx();

    glm::mat4 model;
    glBindVertexArray(sunMoonVaoHandle);
    shader.Use();
    int proHandle=glGetUniformLocation(shader.Program, "projection");
    int viewhandle=glGetUniformLocation(shader.Program, "view");
    glUniformMatrix4fv(proHandle, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewhandle,1, GL_FALSE, glm::value_ptr(view));
    if (proHandle == -1 || viewhandle==-1 ) {
        fprintf(stderr, "Error: can't find texture maps\n");
        exit(1);
    }
    hdrShader.Use();
    //glBindVertexArray(sunMoonVaoHandle);
    int proHandleHDR=glGetUniformLocation(shader.Program, "projection");
    int viewhandleHDR=glGetUniformLocation(shader.Program, "view");
    glUniformMatrix4fv(proHandleHDR, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewhandleHDR,1, GL_FALSE, glm::value_ptr(view));

    //shader.Use();
    int modelhandle = glGetUniformLocation(hdrShader.Program,"model");
    if(modelhandle==-1){
        cout<<"NO MODEL IN HDR VERT"<<endl;
        exit(1);
    }

    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix,glm::vec3(1,1,1));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f, 4.0f, 4.0f));

    glUniformMatrix4fv( modelhandle, 1, false, glm::value_ptr(modelMatrix) );

    shader.Use();
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    // - set lighting uniforms
    for (GLuint i = 0; i < lightPositions.size(); i++)
    {
        glUniform3fv(glGetUniformLocation(shader.Program, ("lights[" + patch::to_string(i) + "].Position").c_str()), 1, &lightPositions[i][0]);
        glUniform3fv(glGetUniformLocation(shader.Program, ("lights[" + patch::to_string(i) + "].Color").c_str()), 1, &lightColors[i][0]);
    }


    // - render tunnel (SPHERE TRANSLATE)
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(xPos,yPos,zPos));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));


    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //If it's moon, render it in a different way
    if(sunOrMoon==1){
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(15,55,5));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));

        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    }
    glUniform1i(glGetUniformLocation(shader.Program, "inverse_normals"), GL_TRUE);

    RenderCube();
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    // 2. Now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    hdrShader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glUniform1i(glGetUniformLocation(hdrShader.Program, "hdr"), hdr);
    glUniform1f(glGetUniformLocation(hdrShader.Program, "exposure"), exposure);
    //RenderQuad();
    //glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
}

void SunMoon::setup_Texture()
{
    glGenFramebuffers(1, &hdrFBO);
    // - Create floating point color buffer

    glGenTextures(1, &colorBuffer);
    //glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, winW, winH, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // - Create depth buffer (renderbuffer)

    glGenRenderbuffers(1, &rboDepth);
    //glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, winW, winH);
    // - Attach buffers
    //glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
SunMoon::~SunMoon()
{
    //dtor
}
