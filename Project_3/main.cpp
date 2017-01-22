

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include "Viewer.hpp"

#include "tiny_obj_loader.h"

#include "InputState.h"

#include <GL/glew.h>

#include <iostream>

#include <stdlib.h>

#include <iomanip>

#include <stdio.h>

#include <fstream>

#include <vector>

#include <stdexcept>

#include <cstring>

#include <GLFW/glfw3.h>
float light[4];

#include "glm/gtc/type_ptr.hpp"

#ifdef __APPLE__

#include <GLUT/glut.h>

#else

#include <GL/glut.h>

#endif


#include "shader.hpp"

#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"

unsigned int winW = 800;

unsigned int winH = 800;

unsigned int ProgramID;

int is_wireframe;

using namespace std;

GLint tick;

float lightX = 0.0f;

float lightY = 5.0f;

float lightZ = -2.0f;

int press;

int press1;

GLuint* TexID;

#define TEX_SIZE 64

#define VALS_PER_VERT 3

#define VALS_PER_COLOUR 4

#define VALS_PER_NORMAL 3

#define VALS_PER_TEX 2

#define CAMERA_SPEED 0.5f



InputState Input;

ObjectViewer *Camera;

int x,y,n;

vector<tinyobj::shape_t> shapes;

vector<tinyobj::material_t>materials;

char* mtl_basepath;
float rotation = 0.0f;
int light_type=0;

float big_x, small_x, big_y, small_y, big_z, small_z;

unsigned int* ObjectVaoHandle;

// calucation average of location where it should appear
inline float axe_max()

{
  float average= max( max((big_x - small_x)/2, (big_y - small_y)/2), (big_z - small_z) / 2);
  return average;

}

//loading obj file and getting edges of model
int load_obj_file(char * str)

{

  std::cout << "Loading " << str << std::endl;
    int i = strlen(str) - 1;
  char* last=strrchr(str,'/');

   string err;
    shapes.clear();
    materials.clear();
  bool ret ;

  cout<<"1"<<endl;

  if(last==NULL)

  {

      ret= tinyobj::LoadObj(shapes, materials, err, str, NULL, true);

  }

  else

      {

          mtl_basepath = new char(last-str+2);

          mtl_basepath[last-str+1]='\0';

          cout<<mtl_basepath<<"here"<<endl;

          strncpy(mtl_basepath,str,last-str+1);

          ret=tinyobj::LoadObj(shapes, materials, err, str, mtl_basepath, true);

      }

  if (!ret) {

      printf("Failed to load/parse .obj.\n");

      return false;

  }

  small_x = small_y = small_z = 0.0f;

  big_x = big_y = big_z =0.0f;

  for (int shape_id = 0; shape_id != shapes.size(); shape_id++)

  {

      vector<float>& positions = shapes[shape_id].mesh.positions;

      for (int i=0, j=0; i<positions.size(); i+=3, j+=1) {

          small_x = min(small_x, positions[i+0]);

          big_x = max(big_x, positions[i+0]);

          small_y = min(small_y, positions[i+1]);

          big_y = max(big_y, positions[i+1]);

          small_z = min(small_z, positions[i+2]);

          big_z = max(big_z, positions[i+2]);
      }

  }

  return 0;

}


int createObjectVAO(unsigned int shape_id)

{

  glGenVertexArrays(1, &ObjectVaoHandle[shape_id]);

  glBindVertexArray(ObjectVaoHandle[shape_id]);

  int vertLoc = glGetAttribLocation(ProgramID, "a_vertex");

  int normLoc = glGetAttribLocation(ProgramID, "a_normal");

  int texLoc = glGetAttribLocation(ProgramID, "a_tex_coord");

  unsigned int buffer[4];

  glGenBuffers(4, buffer);

  // Setting vertex

  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

  glBufferData(GL_ARRAY_BUFFER,

              sizeof(float) * shapes[shape_id].mesh.positions.size(),

              &shapes[shape_id].mesh.positions[0],

              GL_STATIC_DRAW);

  glEnableVertexAttribArray(vertLoc);

  glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

  // Setting normal

  n = shapes[shape_id].mesh.positions.size() / VALS_PER_VERT;

  vector<float>& normals = shapes[shape_id].mesh.normals;

  #ifdef AUTO_NORMAL

      normals.clear();

  #endif

  //calculating normal if it is not provided
  if (normals.size() != n*VALS_PER_NORMAL) {

      cout << "Shape_id: " << shape_id << " - Auto generated normal" << endl;

      int m = shapes[shape_id].mesh.indices.size();

      vector<float>& positions = shapes[shape_id].mesh.positions;

      vector<unsigned int>& indices = shapes[shape_id].mesh.indices;

      normals = vector<float> (n * VALS_PER_NORMAL, 0.f);

      vector<int> count (n, 0);

      for (int index=0; index<m;) {

          int p[3] = {static_cast<int>(indices[index]), static_cast<int>(indices[index+1]), static_cast<int>(indices[index+2])};

          glm::vec3 v[3];

         

          for (int i=0; i!=3; i++) {

              v[i] = glm::vec3(positions[3*p[i]], positions[3*p[i]+1], positions[3*p[i]+2]);

          }

          glm::vec3 normal = glm::normalize(glm::cross((v[1]-v[0]),(v[2]-v[0])));

          for (int i=0; i!=3; i++) {

              normals[3*p[i]] += normal[0];

              normals[3*p[i]+1] += normal[1];

              normals[3*p[i]+2] += normal[2];

              count[p[i]]++;

          }
          index += 3;

      }

      for (int i=0; i!=count.size(); i++) {

          assert(count[i] != 0);

          normals[3*i] /= count[i];

          normals[3*i+1] /= count[i];

          normals[3*i+2] /= count[i];

      }

  }
  //normal attributes are provided
    
  glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);

  glBufferData(GL_ARRAY_BUFFER,

              sizeof(float) * shapes[shape_id].mesh.normals.size(),

              &shapes[shape_id].mesh.normals[0],

              GL_STATIC_DRAW);

  glEnableVertexAttribArray(normLoc);

  glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

    

  // Texture attributes

  n = shapes[shape_id].mesh.positions.size() / VALS_PER_VERT;

  vector<float>& texcoords = shapes[shape_id].mesh.texcoords;

  if (texcoords.size() != n*VALS_PER_TEX) {

      texcoords.clear();

      while (n>0) {

          texcoords.push_back(-1.f);

          texcoords.push_back(-1.f);

          texcoords.push_back(0.f);

          texcoords.push_back(1.f);

          texcoords.push_back(1.f);

          texcoords.push_back(1.f);

          n =n- 3;

      }

  }

  glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);

  glBufferData(GL_ARRAY_BUFFER,

              sizeof(float) * shapes[shape_id].mesh.texcoords.size(),

              &shapes[shape_id].mesh.texcoords[0],

              GL_STATIC_DRAW);

  glEnableVertexAttribArray(texLoc);

  glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

  // Setting  indices

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,

              sizeof(unsigned int) * shapes[shape_id].mesh.indices.size(),

              &shapes[shape_id].mesh.indices[0],

              GL_STATIC_DRAW);

  return 0;


}


void render(double dt)

{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera->update( Input );

    

  glUseProgram(ProgramID);

  int wireframeHandle = glGetUniformLocation(ProgramID, "is_wireframe");

  int lightposHandle = glGetUniformLocation(ProgramID, "a_light_pos");
    
  int mtxHandle = glGetUniformLocation(ProgramID, "transform");
  int modelHandle = glGetUniformLocation(ProgramID, "model_matrix");

  int viewHandle = glGetUniformLocation(ProgramID, "view_matrix");

  int normHandle = glGetUniformLocation(ProgramID, "normal_matrix");

  int modeHandle = glGetUniformLocation(ProgramID, "mode");
    
  //cout<<mtxHandle;
  glm::mat4 transMatrix;
  tick=glGetUniformLocation(ProgramID,"tuk");
  rotation=float(3*dt);

  glm::mat4 modelMatrix = glm::mat4(1.f);

  float scaling = Camera->getScaling();

  modelMatrix = glm::scale(modelMatrix, glm::vec3(scaling, scaling, scaling));

  glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMatrix));

    

  glm::mat4 viewMatrix = Camera->getViewMtx();

  glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMatrix) );

    

  glm::mat3 normalMatrix(viewMatrix * modelMatrix);

  normalMatrix = glm::transpose(glm::inverse(normalMatrix));

  glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normalMatrix));

  // Material:


  int mtlShininessHandle = glGetUniformLocation(ProgramID, "mtl_shininess");

  int lightPosHandle = glGetUniformLocation(ProgramID, "a_light_pos");

    
  int mtlAmbientHandle = glGetUniformLocation(ProgramID, "mtl_ambient");

  int mtlDiffuseHandle = glGetUniformLocation(ProgramID, "mtl_diffuse");

  int mtlSpecularHandle = glGetUniformLocation(ProgramID, "mtl_specular");

  float lightPos[4];

  if (light_type == 1) {

      lightPos[0] = -0.05f;

      lightPos[1] = -1.f;

      lightPos[2] = -0.5f;

      lightPos[3] = 0.f;


  }

   else if(light_type == 2)

   {

    lightPos[0] = 0.f;

    lightPos[1] = 0.f;

    lightPos[2] = 0.f;

    lightPos[3] = 1.f;
     
   }
   
   else if(light_type == 3)
     
   {
   
    lightPos[0] =rotation;
     
    lightPos[1] = 0.f;
     
    lightPos[2] = 0.f;
     
    lightPos[3] =1.f;
    transMatrix = glm::rotate(transMatrix,
                              rotation,
                              glm::vec3(0.0f, 0.0f, 1.0f));

   }

  else {

      lightPos[0] = 0.f;

      lightPos[1] = 0.f;

      lightPos[2] = 0.f;

      lightPos[3] = 1.f;

  }
    
  glUniformMatrix4fv( mtxHandle, 1, false, glm::value_ptr(transMatrix) );
  glUniform4fv(lightPosHandle, 1, lightPos);

  glUniformMatrix4fv( mtxHandle, 1, false, glm::value_ptr(transMatrix) );

  glUniform1i(modeHandle, 0);

  glUniform1i(wireframeHandle, is_wireframe);

    

  for (int i=0; i!=shapes.size(); i++) {

      glBindVertexArray(ObjectVaoHandle[i]);

      int material_id = shapes[i].mesh.material_ids[0];

     

      glBindTexture(GL_TEXTURE_2D, TexID[material_id]);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

     

      glUniform3fv(mtlAmbientHandle, 1, materials[material_id].ambient);

      glUniform3fv(mtlDiffuseHandle, 1, materials[material_id].diffuse);

      glUniform3fv(mtlSpecularHandle, 1, materials[material_id].specular);

      glUniform1f(mtlShininessHandle, materials[material_id].shininess);

      glDrawElements(GL_TRIANGLES, shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, 0);

  }

  Input.deltaX = Input.deltaY = 0;

    

  glFlush();


    

}

//Loading the texture from image
int load_texture()

{

  TexID = new GLuint[shapes.size()];

  glActiveTexture(GL_TEXTURE0);

  glGenTextures(shapes.size(), TexID);

  cout<<"5"<<endl;

  for (unsigned int id=0; id!=materials.size(); id++)
  {

      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );

      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

     

      if (!materials[id].diffuse_texname.empty()) {

          string full_name = string(mtl_basepath) + materials[id].diffuse_texname;

          unsigned char *data = stbi_load(full_name.c_str(), &x, &y, &n, 0);
         
          //No texture files
          if (data == NULL)

          {

              fprintf(stderr, "Texture file <%s> is NULL\n");

              return -1;

          }

         

          if (n == 3)  {

              glBindTexture( GL_TEXTURE_2D, TexID[id]);

              glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

          }

          else {

              fprintf(stderr, "Image pixels are not RGB, actual mode is %d!\n", n);

              return -1;

          }

          stbi_image_free(data);

      } else {

          GLubyte myimage[TEX_SIZE][TEX_SIZE][3];

          for (int i=0; i<TEX_SIZE; i++)

              for (int j=0; j<TEX_SIZE; j++)

              {

                  myimage[i][j][0] = myimage[i][j][1] = myimage[i][j][2] = 255;

              }

          glBindTexture( GL_TEXTURE_2D, TexID[id]);

          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, myimage);

      }

      glGenerateMipmap(GL_TEXTURE_2D);

  }

  return 0;

}

int setup()

{

  glm::mat4 projection;

  projection = glm::perspective( 2*atan(axe_max() / (axe_max() + 1.f)), float(winW) / float(winH), 0.005f, 10000.0f);

    

  // Load it to the shader program

  int projHandle = glGetUniformLocation(ProgramID, "projection_matrix");

  if (projHandle == -1) {

      std::cerr << "Uniform: projection_matrix was not an active uniform label\n";

  }

  glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );

  cout<<"4"<<endl;

  //Load textures

  if(load_texture()!=0)

      return -1;

    

  ObjectVaoHandle = new unsigned int[shapes.size()];

  for(int i=0;i!=shapes.size();i++)

  {

      if(createObjectVAO(i)==-1)

      {

          return -1;

      }

  }

  return 0;

}


void mouse_pos_callback(GLFWwindow* window, double x, double y)

{

  int xDiff=x-Input.prevX;

  int yDiff=y-Input.prevY;

  Input.deltaX=Input.deltaX+xDiff;

  Input.deltaY=Input.deltaY+yDiff;

  Input.prevX=x;

  Input.prevY=y;

    

}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)

{

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {

      Input.rMousePressed = true;

      Input.prevX=x;

      Input.prevY=y;

     

  }

  else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {

      Input.rMousePressed = false;

     

  }

  else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

      Input.lMousePressed = true;

      Input.prevX=x;

      Input.prevY=y;

     

  }

  else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {

      Input.lMousePressed = false;

  }

}

void update_light(int light_type)

{

    

  int lightSpecularHandle = glGetUniformLocation(ProgramID, "light_specular");
  int lightHandle = glGetUniformLocation(ProgramID, "light_type");

  int lightAmbientHandle = glGetUniformLocation(ProgramID, "light_ambient");

  int lightDiffuseHandle = glGetUniformLocation(ProgramID, "light_diffuse");
  glUniform1i(lightHandle, light_type);

  cout<<light_type<<endl;
//case 1 blue diffuse light
  switch (light_type) {

      case 0:

      {

          float lightambient[3] = {1.0f, 1.0f, 1.0f};

          float lightdiffuse[3] = {0.0f, 0.0f, 0.0f};

          float lightspecular[3] = {0.0f, 0.0f, 0.0f};

         

          glUniform3fv(lightAmbientHandle, 1, lightambient);

          glUniform3fv(lightDiffuseHandle, 1, lightdiffuse);

          glUniform3fv(lightSpecularHandle, 1, lightspecular);

          break;

      }
//case 2 point white light
      case 1:

      {

          float lightambient[3] = {1.0f, 1.0f, 1.0f};

          float lightdiffuse[3] = {0.5f, 0.5f, 1.0f};

          float lightspecular[3] = {1.0f, 1.0f, 1.0f};

         

          glUniform3fv(lightAmbientHandle, 1, lightambient);

          glUniform3fv(lightDiffuseHandle, 1, lightdiffuse);

          glUniform3fv(lightSpecularHandle, 1, lightspecular);

          break;

         

      }

      case 2:

      {

          float lightambient[3] = {1.0f, 1.0f, 1.0f};

          float lightdiffuse[3] = {1.0f, 1.0f, 1.0f};

          float lightspecular[3] = {1.0f, 1.0f, 1.0f};

         

          glUniform3fv(lightAmbientHandle, 1, lightambient);

          glUniform3fv(lightDiffuseHandle, 1, lightdiffuse);

          glUniform3fv(lightSpecularHandle, 1, lightspecular);

          break;

      }

         
//case 3 yellow light rotation
      case 3:

      {

         

          float lightambient[3] = {0.5f, 0.5f, 0.5f};

          float lightdiffuse[3] = {0.5f, 0.5f, 0.0f};

          float lightspecular[3] = {0.5f, 0.5f, 0.0f};

          glUniform3fv(lightAmbientHandle, 1, lightambient);

          glUniform3fv(lightDiffuseHandle, 1, lightdiffuse);

          glUniform3fv(lightSpecularHandle, 1, lightspecular);

          break;

      }

  }

}


void key_callback(GLFWwindow* window,

                int key, int scancode, int action, int mods)

{

 

  // Close the application by pressing Esc

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)

  {

      glfwSetWindowShouldClose(window, GL_TRUE);

  }

  // Toggle between eye and object coordinates by pressing e

    

  else if ( key ==GLFW_KEY_S && action == GLFW_PRESS)

  {

      is_wireframe=!is_wireframe;

      if(is_wireframe==0)

      {

          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


      }

  


  }

  else if(  key ==GLFW_KEY_D && action == GLFW_PRESS && is_wireframe==1)

  {

      press=1+press;

      cout<<"tick -"<<tick<<endl;

      if(press%3==0)

      {

          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

          cout<<"tick1 = "<<endl;

          glUniform1f(tick,0);

      }

      else if ( press%3==1)

      {

          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

          cout<<"tick2 = "<<endl;

          glUniform1f(tick,1);

      }

      else

      {

          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

          cout<<"tick3 = "<<endl;

          glUniform1f(tick,2);

      }

  }

  else if ( key ==GLFW_KEY_L && action == GLFW_PRESS && is_wireframe==0)

  {

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      light_type = (light_type + 1) % 4;

      update_light(light_type);

  }

  else if ( key==GLFW_KEY_UP && action == GLFW_PRESS)

  {

      Camera->moveFB(CAMERA_SPEED);

  }

  else if ( key==GLFW_KEY_DOWN && action == GLFW_PRESS)

  {

      Camera->moveFB(-CAMERA_SPEED);

  }

  else if ( key==GLFW_KEY_LEFT && action == GLFW_PRESS)

  {

      Camera->moveLR(-CAMERA_SPEED);

  }

  else if ( key==GLFW_KEY_RIGHT && action == GLFW_PRESS)

  {

      Camera->moveLR(CAMERA_SPEED);

  }

  else if ( key ==GLFW_KEY_B && action == GLFW_PRESS)

  {

      press1++;

      {

          if(press1%4==0)

          {

              glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

          }

          else if(press1%4==1)

          {

              glClearColor(0.8f, 0.0f, 0.0f, 1.0f);

          }

          else if(press1%4==2)

          {

              glClearColor(1.0f, 0.5f, 0.0f, 1.0f);

          }

          else

          {

              glClearColor(0.5f, 0.0f, 1.0f, 1.0f);

          }

      }

     

     

  }


}



/**

 * Error callback for GLFW. Simply prints error message to stderr.

 */

void error_callback(int error, const char* description)

{

  std::cerr << description;

}



int main(int argc,char ** argv)

{

  assert(argc==2);

    if(load_obj_file(argv[1])!=0)

      return -1;
    

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

  cout<<"2"<<endl;

  // Create the window and OpenGL context

  window = glfwCreateWindow(winH, winW, "Tex coords", NULL, NULL);

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

      std::cerr << "Failed to initialize GLEW\n";

      exit(1);

  }

    

  // Initialise OpenGL state

  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  glEnable(GL_DEPTH_TEST);

  ProgramID=LoadShaders("light-texture.vert","light-texture.frag");

  if(ProgramID==0)

  {

      fprintf(stderr,"Can't compile shaders!\n");

      return 1;

  }

  glUseProgram(ProgramID);

    

  if(setup()!=0)

  {

      cout<<"3"<<endl;

      return 1;

  }

    

  is_wireframe=1;

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    

  glm::vec3 eye((small_x + big_x) / 2, (small_y + big_y) / 2, max(big_z + 3.f, axe_max() + 3.f));

  glm::vec3 at((small_x + big_x) / 2, (small_y + big_y) / 2, (small_z + big_z) / 2);

  glm::vec3 up(0.0f, 1.0f, 0.0f);

    

  Camera= new ObjectViewer(eye,at,up);

  update_light(light_type);

    

  glfwSetCursorPosCallback(window, mouse_pos_callback);

  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwSetKeyCallback(window, key_callback);

  double start = glfwGetTime();

  double now;

    

  while (!glfwWindowShouldClose(window))

  {

      now = glfwGetTime();
      //cout<<now<<endl;
      render(now - start);

     

      glfwSwapBuffers(window);

      glfwPollEvents();

  }

    

  // Clean up

  glfwDestroyWindow(window);

  glfwTerminate();

  exit(0);

    

  return 0;

}





