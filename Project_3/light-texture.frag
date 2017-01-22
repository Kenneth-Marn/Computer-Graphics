
#version 330


uniform int is_wireframe;
uniform int has_texture;
uniform float tuk;
uniform int mode;

uniform int light_type;

uniform vec3 light_ambient;     // Light ambient RGBA values
uniform vec3 light_diffuse;     // Light diffuse RGBA values  
uniform vec3 light_specular;    // Light specular RGBA values

uniform vec3 mtl_ambient;
uniform vec3 mtl_diffuse;
uniform vec3 mtl_specular;
uniform float mtl_shininess;

in vec4 light_pos;
in vec2 st;
in vec4 vertex;
in vec3 normal;
in vec3 original_vertex;
in vec4 normal_1;
uniform sampler2D texMap;
out vec4 fragColour;

// This function computes the Phong lit colour for a vertex
// Parameters are the vertex position and normal in eye space.
vec3 phongPointLight(in vec4 position, in vec3 norm)
{
    // s is the direction from the light to the vertex
    vec3 s;
    if (light_type == 1)
    	s = normalize(vec3(-light_pos));
    else 
    	s = normalize(vec3(light_pos - position));

    // v is the direction from the eye to the vertex
    vec3 v = normalize(-position.xyz);

    // r is the direction of light reflected from the vertex
    vec3 r = reflect( -s, norm );
    
    vec3 ambient = light_ambient * mtl_ambient;
	
    // The diffuse component
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = light_diffuse * mtl_diffuse * sDotN;

    // Specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = light_specular * mtl_specular *
            pow( max( dot(r,v), 0.0 ), mtl_shininess );

    return ambient + diffuse + spec;
}

void main(void)
{
	switch (mode) {
	case 0:
		if (is_wireframe > 0)
        {
            if(tuk==0)
            {
                fragColour = vec4(0.f, 1.f, 0.f, 1.f);
            }
            else if(tuk==1)
            {
                fragColour=vec4(mtl_diffuse, 1.f);
            }
            else if(tuk==2)
            {
                fragColour=vec4(normal,1.0f);
            }
        }
		else {
            
                if(light_type == 0)
                {
                    fragColour=texture(texMap, st);
                }
            else{
                    fragColour = texture(texMap, st) * vec4(phongPointLight(vertex, normal), 1.f);
                }
        }
		break;

	}

}
