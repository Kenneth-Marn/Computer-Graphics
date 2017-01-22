
uniform int mode;

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat3 normal_matrix;

uniform vec4 light_pos;

uniform vec3 light_ambient;     // Light ambient RGBA values
uniform vec3 light_diffuse;     // Light diffuse RGBA values  
uniform vec3 light_specular;    // Light specular RGBA values

uniform vec3 mtl_ambient;
uniform vec3 mtl_diffuse;
uniform vec3 mtl_specular;

in vec3 a_vertex;
in vec3 a_normal;

out vec3 original_vertex;

void main(void)
{
	vec3 eyeNorm = normalize( normal_matrix * a_normal);

	original_vertex = a_vertex;

	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(a_vertex, 1.0);
}
