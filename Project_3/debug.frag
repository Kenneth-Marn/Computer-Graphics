
#version 330

// manyAttributes.fp
// An example of using interpolated values from the previous stage

uniform int mode;

in vec3 original_vertex;
out vec4 fragColour;

void main(void)
{
	switch (mode) {
	case 0:
		fragColour = vec4(1.f, 1.f, 0.f, 1.f);
		break;
	case 1:
		if (original_vertex.x > 0.f)
			fragColour = vec4(1.f, 0.f, 0.f, 1.f);
		else if (original_vertex.y > 0.f)
			fragColour = vec4(1.f, 1.f, 0.f, 1.f);
		else if (original_vertex.z > 0.f)
			fragColour = vec4(0.f, 0.f, 1.f, 1.f);
		else
			fragColour = vec4(1.f, 1.f, 1.f, 1.f);
		break;
	}

}
