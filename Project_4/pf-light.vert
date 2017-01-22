
#version 330 core

uniform mat4 projection;
uniform mat4 model;
//uniform mat4 view;
uniform mat4 DepthBiasMVP;
uniform mat3 normal_matrix;

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;

out vec4 vertex; // vertex position in eye space
out vec3 normal; // the eye space normal
out vec2 tex_coord;// texture coords
out vec4 ShadowCoord; // Vertex position in light space

//for object coords
out vec4 pos;

//Textured height map
out vec4 colour;


// fog position based on camera
uniform vec3 fogPos;
out vec3 dist;
out vec4 glpos;
void main(void)
{

    pos = vec4(a_vertex, 1.0);
    vertex = model * vec4(a_vertex, 1.0);
	normal = normal_matrix * a_normal;
	//For textured height map
	tex_coord = a_tex_coord;
	colour=  vec4(20* pos.y,20* pos.y,20* pos.y,1.0);

    // get distance between camera and obj
    dist = fogPos - a_vertex;

    // Same, but with the light's view and projection matrix
    // This is the vertex's position relative to the light
    ShadowCoord = DepthBiasMVP * vec4(a_vertex,1.0);

	gl_Position = projection * vertex;
	glpos=gl_Position;
}
