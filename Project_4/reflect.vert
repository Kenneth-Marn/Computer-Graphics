
#version 330

layout (location = 0) in vec3 a_vertex; 
layout (location = 1) in vec3 a_normal; 

uniform mat4 projection;
uniform mat4 view;

out vec3 pos;
out vec3 normal;

void main(void)
{
    // Normal and pos are in camera coordinates
    // Normal uses the top 3x3 of the view matrix, removing any translation
    // See lighting example for more details.
    normal = mat3(view) * a_normal;
    pos = vec3(view * vec4(a_vertex, 1.0));
                  
	gl_Position = projection * view * vec4(a_vertex, 1.0);
}
