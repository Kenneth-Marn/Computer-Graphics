
#version 330

// Position and colour variables. 1 per vertex.
layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_colour;
layout (location = 2) in vec2 a_texcoord;




uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal_matrix;

out vec4 colour;
out vec2 Texcoord;

// This colour attribute will be sent out per-vertex and automatically
// interpolated between vertices connected to the same triangle
out vec4 pos;

void main(void)
{
	pos = vec4(a_vertex, 1.0);	// We simply pass the colour along to the next stage
    colour=  vec4(20* pos.y,20* pos.y,20* pos.y,1.0);
    Texcoord = a_texcoord;
    //colour = vec4(abs(a_colour.x)+0.1,abs(a_colour.y)+0.1,abs(a_colour.z)+0.1,1.0);
    // a_vertex is vertex position in object coordinates
    // gl_position is vertex position in camera coordinates
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
