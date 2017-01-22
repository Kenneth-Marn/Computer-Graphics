#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
//uniform mat4 modelview_matrix;
vec4 vec;
uniform bool inverse_normals;

void main()
{
    vec=vec4(position, 1.0f);
    gl_Position =  projection*view*model *vec;
    //gl_Position=modelview_matrix*gl_Position;
    vs_out.FragPos = vec3( vec4(position, 1.0));
    vs_out.TexCoords = texCoords;

    vec3 n = inverse_normals ? -normal : normal;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalize(normalMatrix * n);

}
