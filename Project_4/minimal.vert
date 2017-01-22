
#version 330

// Position and colour variables. 1 per vertex.
// These are buffers which stores the values that we created from the main program.
layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec4 a_colour;

// This colour attribute will be sent out per-vertex and automatically
// interpolated between vertices connected to the same triangle
out vec4 colour;
out vec3 coordinate;


uniform mat4 transform;
void main(void)
{
	colour = a_colour;	// We simply pass the colour along to the next stage
    // gl_position is a built in variable to store vertex position
    // It has 4 coordinates for reasons we'll see later on.
	//gl_Position = vec4(a_vertex, 1.0);


    //pass the vertex buffer to coordinate which will be passed to the fragment shader
    //to give color at certain point
    coordinate = a_vertex;

	// Multiply our vertex positions by the vtx transform set in main application
	gl_Position = transform * vec4(a_vertex, 1.0);

}
