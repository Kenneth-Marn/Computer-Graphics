
#version 330

// Just interpolate the lit colour computed for each vertex

in vec4 litColour;

out vec4 fragColour;

void main(void)
{
	fragColour = litColour;
}
