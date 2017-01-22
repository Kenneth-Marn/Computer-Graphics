/*********************************************************
Version 1.000

Code provided by Michael Hemsley and Anthony Dick
for the course 
COMP SCI 3014/7090 Computer Graphics
School of Computer Science
University of Adelaide

Permission is granted for anyone to copy, use, modify, or distribute this
program and accompanying programs and documents for any purpose, provided
this copyright notice is retained and prominently displayed, along with
a note saying that the original programs are available from the aforementioned 
course web page. 

The programs and documents are distributed without any warranty, express or
implied.  As the programs were written for research purposes only, they have
not been tested to the degree that would be advisable in any important
application.  All use of these programs is entirely at the user's own risk.
*********************************************************/

// May need to change this to 130 to compile on older lab machines
#version 330



uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat3 normal_matrix;
uniform vec4 a_light_pos;
uniform int light_type;
uniform int tik;
uniform mat4 transform;

in vec3 a_vertex;
in vec2 a_tex_coord;
in vec3 a_normal;

out vec2 st;
out vec3 original_vertex;
out vec4 vertex;
out vec3 normal;
out vec4 light_pos;
out vec4 normal_1;
out vec4 twat;

void main(void)
{
    st = vec2(a_tex_coord.x,1.0f-a_tex_coord.y);
    original_vertex = a_vertex;
    vertex = view_matrix * model_matrix * vec4(a_vertex, 1.0);
    normal = normalize( normal_matrix * a_normal );
    normal_1=vec4(a_normal,1.0f);
    if (light_type == 0) {
        light_pos = view_matrix * a_light_pos;
    }
    else if(light_type==3)
    {
        light_pos=a_light_pos*transform;
    }
    else
        light_pos = a_light_pos;

    gl_Position = projection_matrix * vertex;
}


