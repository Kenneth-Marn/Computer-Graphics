
#version 330

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


// Notice 3D tex coords for a cube map
// Tex coord is a direction in 3D
out vec3 texCoord;

//For blending
//out float alpha;
//out vec4 colour;
///For light & Shadow////////////////////////
//uniform mat4 projection_matrix;
//uniform mat4 modelview_matrix;
uniform mat4 DepthBiasMVP;
uniform mat3 normal_matrix;

layout (location = 2) in vec2 a_tex_coord;

out vec4 vertex; // vertex position in eye space
out vec3 normal; // the eye space normal
out vec2 tex_coord;// texture coords
out vec4 ShadowCoord; // Vertex position in light space
void main(void)
{

	texCoord = a_vertex; // use position as texture coord
    //normal = normalize(normal_matrix * a_normal);
    vertex = view * model * vec4(a_vertex, 1.0);
    //tex_coord = a_tex_coord;

    //colour = vec4(a_normal, alpha);

    // Same, but with the light's view and projection matrix
    // This is the vertex's position relative to the light
    //ShadowCoord = DepthBiasMVP * vec4(a_vertex,1.0);
    gl_Position = projection * vertex;
}
