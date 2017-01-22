
#version 330

in vec4 colour; // The per-vertex, colour attribute from the previous stage.
in vec3 coordinate;

// The final colour we will see at this location on screen
out vec4 fragColour;
//time variable to indiccate transformation of certain colors
uniform float time;
void main(void)
{
    // We now just have to set the already smoothed colour as our frag colour


    //Inner Box Colored Red
    if(coordinate.x>-0.1f && coordinate.y>-0.1f &&
       coordinate.x<0.1f && coordinate.y<0.1f ){
        if(time>3){
            fragColour = vec4(1.0f,0,0,1);
        }else{
            fragColour = vec4(0.0f,0,0,1);
        }
    }
    //Blue Outline
    else if(coordinate.x>-0.2f && coordinate.y>-0.2f &&
       coordinate.x<0.2f && coordinate.y<0.2f){
        if(time>3.7f){
            fragColour = vec4(1.0f,0,0,1);
        }else{
            fragColour = vec4(0.0f,0.0f,1,1);
        }
    }
    //Pink Outline
    else if(coordinate.x>-0.3f && coordinate.y>-0.3f &&
       coordinate.x<0.3f && coordinate.y<0.3f ){
        if(time>4.5f){
            fragColour = vec4(1.0f,0,0,1);
        }
        else{
            fragColour = vec4(1.0f,0,1,1);
        }
    }
    else if(coordinate.x>-0.4f && coordinate.y>-0.4f &&
       coordinate.x<0.4f && coordinate.y<0.4f ){
        if(time>5.2f){
            fragColour = vec4(1.0f,0,0,1);
        }else{
        fragColour = vec4(0.0f,0.2f,0,1);
        }
    }
    else if(coordinate.x>-0.5f && coordinate.y>-0.5f &&
       coordinate.x<0.5f && coordinate.y<0.5f ){
        if(time>6.0f){
            fragColour = vec4(1.0f,0,0,1);
        }else{
        fragColour = vec4(0.0f,0.7f,0,1);
        }
    }
    //Purple
    else{
        fragColour = vec4(0.3f,0,0.6f,1);
    }
}
