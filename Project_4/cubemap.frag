
#version 330

in vec3 texCoord;

// The final colour we will see at this location on screen
out vec4 fragColour;

uniform samplerCube skybox;
uniform samplerCube dayBox;
uniform samplerCube nightBox;

uniform float alphaDark;
uniform int tod;
//for blending
//in vec4 colour;
uniform float alpha;
void main(void)
{

    vec4 day = texture(dayBox,texCoord);
    vec4 night = texture(nightBox,texCoord);
    vec4 sky = texture(skybox,texCoord);
    // Texture lookup, just as for 2D texture mapping
    //fragColour = texture(skybox, texCoord);
    //fragColour = night;
    fragColour = sky;
    //fragColour.a=alpha;

    //tod==0 -> daytime
    if(tod==0){
        fragColour.a=(alpha);
    }
    else{
        fragColour.a=(alphaDark);
    }
    //fragColour.a=1.0f;
}
