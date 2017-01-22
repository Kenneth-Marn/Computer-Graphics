
#version 330

in vec3 pos; // Interpolated vertex position, in camera coords
in vec3 normal; // interpolated normal in camera coords

// The final colour we will see at this location on screen
out vec4 fragColour;

uniform mat4 view;
uniform samplerCube skybox;

void main(void)
{
    // Calculate the reflection direction in camera coords
    vec3 v = normalize(pos);
    vec3 r = reflect(v, normalize(normal));

    // Convert back to world coordinates to look up reflected colour
    vec3 r_world = vec3(inverse(view) * vec4(r, 0.0));   
    fragColour = texture(skybox, r_world);

}
