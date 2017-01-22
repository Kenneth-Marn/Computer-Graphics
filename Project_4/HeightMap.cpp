
#include "HeightMap.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>



#include <GL/glew.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define CUBE_NUM_TRIS 2
#define VALS_PER_VERT 3
#define NUM_TEXTURES 4
#define VALS_PER_TEX 2


/**
 * Creates a new vertex array object for a cube
 * and loads in data into a vertex attribute buffer
 */

HeightMap::HeightMap( int id)
{
    struct imgInfo img1;
    struct imgInfo img2;
    struct imgInfo img3;
    struct imgInfo img4;
    img1.TexNames="HeightMap/img/sand.jpg";
    img2.TexNames="HeightMap/img/grass.jpg";
    img3.TexNames="HeightMap/img/rock.png";
    img4.TexNames="HeightMap/img/snow.jpg";

    Images.push_back(img1);
    Images.push_back(img2);
    Images.push_back(img3);
    Images.push_back(img4);

    readingIMG();
    vaoHeightHandle = createHeightMapVAO(id);
}

/**Textures**/

void HeightMap::genTextures(int programID){

    GLuint tex[NUM_TEXTURES];
    glGenTextures(NUM_TEXTURES, tex);
    for(int i=0; i<NUM_TEXTURES; i++){
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D, tex[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



        unsigned char* image =stbi_load(
                                        Images.at(i).TexNames.c_str(), /*char* filepath */
                                        &Images.at(i).width, /*The address to store the width of the image*/
                                        &Images.at(i).height, /*The address to store the height of the image*/
                                        &Images.at(i).channels,  /*Number of channels in the image*/
                                        0   /*Force number of channels if > 0*/
                                        );




        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Images.at(i).width, Images.at(i).height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        char buffer [1];
        //std::string i_str=itoa(i+1,buffer,10);
        std::stringstream ss;
        ss << (i+1);
        std::string str = ss.str();
        std::string i_str=str;
        std::cout<<i_str<<std::endl;
        std::string texUniName="texSample"+i_str;
        std::cout<<texUniName<<std::endl;
        glUniform1i(glGetUniformLocation(programID, texUniName.c_str()), i);


        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);
    }

}

 void HeightMap::readingIMG(){
    int x, y, n;
    std::cout<<"HAHAHAHA"<<std::endl;
    IMGdata = stbi_load(            "HeightMap/img/hm4.png", /*char* filepath */
                                    &x, /*The address to store the width of the image*/
                                    &y, /*The address to store the height of the image*/
                                    &n,  /*Number of channels in the image*/
                                    0   /*Force number of channels if > 0*/
    );



    height=x;
    width=y;
    channels=n;
    std::cout<<x<<std::endl;
    std::cout<<y<<std::endl;
    std::cout<<channels<<std::endl;
    std::cout<<"DONE"<<std::endl;
}


int HeightMap::createHeightMapVAO(int programID)
{
    // Cube vertex data

    if(height>width){
        numeratorW=(float)width/height;
        numeratorH=1.0;

    }else{
        numeratorH=(float)height/width;
        numeratorW=1.0;

    }
    std::cout<<numeratorW<<" "<<numeratorH<<std::endl;
    std::vector<float> vertices;

    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            float heightRatio=1.0/255.0;
            float yVal=0.05*heightRatio*(float)IMGdata[3*((int)(i*width)+j)];

            vertices.push_back(j*(numeratorW/(width-1)));
            vertices.push_back(yVal);
            vertices.push_back(i*(numeratorH/(height-1)));

            //std::cout<<j*(1.0/(width-1))<<" "<<0.0<<" "<<i*(1.0/(height-1))<<std::endl;
            //std::cout<<(float)IMGdata[3*((int)(i*width)+j)]<<std::endl;
            //std::cout<<yVal<<std::endl;
        }
    }
    std::cout<<"All goods"<<std::endl;
    /*
    for(int i=0;i<513*513;i++){
    for(int j=0;j<3;j++){
        int num=(float)IMGdata[(3*i)+j];

    }
    }*/
    stbi_image_free(IMGdata);




    // 12 triangles - 2 per face of the cube
    std::vector<unsigned int> indices;
    for(int i=0; i<height-1; i++){
        for(int j=0; j<width-1; j++){                         //*******//
            indices.push_back((i*width)+j);             //v0    //     //
                                                        //v3      //   //
            indices.push_back(((i+1)*width)+(j+1));     //v1        // //
            indices.push_back((i*width)+(j+1));                                                           //
                                                            //
                                                            // //
            indices.push_back((i*width)+j);                 //   //
            indices.push_back(((i+1)*width)+(j));           //     //
            indices.push_back(((i+1)*width)+(j+1));         //*******//

            //std::cout<<(i*width)+j<<std::endl;
            //std::cout<<(i*width)+(j+1)<<std::endl;
            //std::cout<<((i+1)*width)+(j+1)<<std::endl;

            //std::cout<<(i*width)+j<<std::endl;
            //std::cout<<((i+1)*width)+(j)<<std::endl;
            //std::cout<<((i+1)*width)+(j+1)<<std::endl<<std::endl;
        }
    }




    /**calculating Normals**/
    std::vector<float> normals;

    for(unsigned int i = 0; i < vertices.size(); i++){
        normals.push_back(0);
    }
    /*
    const glm::vec3 UP( 0.0f, 1.0f, 0.0f );
    std::cout<<"Indices"<<indices.size()<<std::endl;
    std::cout<<"NORM "<<normals.size()<<std::endl;

    for ( unsigned int i = 0; i < normals.size(); i+=3 )
    {
        glm::vec3 normal = glm::normalize( glm::vec3(normals[i+0],normals[i+1],normals[i+2] ) );
        normals[(i)+0] = normal.x;
        normals[(i)+1] = normal.y;
        normals[(i)+2] = normal.z;
        /*
        #if ENABLE_SLOPE_BASED_BLEND
            float fTexture0Contribution = glm::saturate( glm::dot( m_NormalBuffer[i], UP ) - 0.1f );
            m_ColorBuffer[i] = glm::vec4( fTexture0Contribution, fTexture0Contribution, fTexture0Contribution, m_ColorBuffer[i].w );
        #endif
        /
        /

    }*/

		std::vector<int> count (vertices.size()/3, 0);
		for (int index=0; index<indices.size();) {

			int p[3] = {indices[index], indices[index+1], indices[index+2]};
			glm::vec3 v[3];

			for (int i=0; i!=3; i++) {
				v[i] = glm::vec3(vertices[3*p[i]], vertices[3*p[i]+1], vertices[3*p[i]+2]);
			}
			glm::vec3 normal =glm::cross(glm::normalize(v[1]-v[0]),glm::normalize(v[2]-v[0]));

			for (int i=0; i!=3; i++) {
				normals[3*p[i]] += normal[0];
				normals[3*p[i]+1] += normal[1];
				normals[3*p[i]+2] += normal[2];
				count[p[i]]++;
			}


			index += 3;
		}
		for (int i=0; i!=count.size(); i++) {
			//assert(count[i] != 0);
			normals[3*i] /= count[i];
			normals[3*i+1] /= count[i];
			normals[3*i+2] /= count[i];
		}


    /****************************************************************************/

    /**TEXTURE COORDINATES**/

    std::vector<float> texCoords;
    int n_repeat_texture=400;

    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            if(i%2==0){
                texCoords.push_back(j*(1.0/width));
                texCoords.push_back(i*(1.0/height));

            }else{

                texCoords.push_back(j*(1.0/width));
                texCoords.push_back(i*(1.0/height));


            }

        }


    }
    /*
    for(int i =0;i<1200;i+=2){
        std::cout<<"("<<texCoords.at(i)<<", "<< texCoords.at(i+1)<<") "<<std::endl;;
    }
    */
    /****************************************************************************/

    unsigned int* Indices = &indices.at(0);
    float* Vertices = &vertices.at(0);
    float* Normals= &normals.at(0);
    float* TexCoords = &texCoords.at(0);


    glUseProgram(programID);
	glGenVertexArrays(1, &vaoHeightHandle);
	glBindVertexArray(vaoHeightHandle);

	unsigned int buffer[4];
	glGenBuffers(4, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float)*height*width*3, Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
	//set normal attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float)*normals.size(), Normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	//set tex coordinates
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float)*texCoords.size(), TexCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

 	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int)*6*((height-1)*(width-1)), Indices, GL_STATIC_DRAW);


    // Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	genTextures(programID);

	return vaoHeightHandle;
}


/**
 Draw the table as a set of scaled blocks.
**/

void HeightMap::render(int programID)
{
	glUseProgram(programID);
    //glActiveTexture(GL_TEXTURE0+5);
	int modelUniformHandle = glGetUniformLocation(programID, "model");
	if (modelUniformHandle == -1){
		exit(1);
	}
 	glBindVertexArray(vaoHeightHandle);

    // We have separated modelling from viewing.
    // Viewing (i.e. placing the camera relative to the entire table)
    // is handled in the viewer class.
    glm::mat4 model;
    model = glm::scale( model, glm::vec3(250.0, 250.0, 250.0) );
    model = glm::translate( model, glm::vec3(-numeratorW/2, 0.0, -numeratorH/2) );
    // now apply the scales to each cube forming the table
    // First the table top
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
    glDrawElements(GL_TRIANGLES, (width-1)*(height-1)*6, GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // cube on the table... or other objects... can go here


	//glBindVertexArray(0);

	//glFlush();

}

