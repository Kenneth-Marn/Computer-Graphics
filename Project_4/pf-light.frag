
#version 330 core
//Additional Attributes//
in vec4 pos; // Interpolated vertex position, in object coords
in vec4 glpos;

uniform int mapRender;
uniform int sphereRender;


//  **********************//


in vec4 vertex;
in vec3 normal;
in vec2 tex_coord;
in vec4 ShadowCoord;

out vec4 fragColour;
out float fragmentdepth;

uniform vec4 light_pos;

uniform vec3 light_ambient;     // Light ambient RGBA values
uniform vec3 light_diffuse;     // Light diffuse RGBA values
uniform vec3 light_specular;    // Light specular RGBA values

uniform vec3 mtl_ambient;  // Ambient surface colour
uniform vec3 mtl_diffuse;  // Diffuse surface colour
uniform vec3 mtl_specular; // Specular surface colour

// The depth buffer from the shadow render pass
uniform sampler2D gShadowMap;

// 0 means we draw the scene normally, 1 means drawing to FBO
uniform int shadow;

const float shininess = 32;

// fog
//uniform vec3 fogPos;
in vec3 dist;


/**  Textured height map attribute*/
uniform sampler2D texSample1;
uniform sampler2D texSample2;
uniform sampler2D texSample3;
uniform sampler2D texSample4;


float alpha;


vec4 hash4( vec2 p ) { return fract(sin(vec4( 1.0+dot(p,vec2(37.0,17.0)),
                                              2.0+dot(p,vec2(11.0,47.0)),
                                              3.0+dot(p,vec2(41.0,29.0)),
                                              4.0+dot(p,vec2(23.0,31.0))))*103.0); }


vec4 textureNoTile( sampler2D samp, in vec2 uv )
{
    ivec2 iuv = ivec2( floor( uv ) );
     vec2 fuv = fract( uv );

    // generate per-tile transform
    vec4 ofa = hash4( iuv + ivec2(0,0) );
    vec4 ofb = hash4( iuv + ivec2(1,0) );
    vec4 ofc = hash4( iuv + ivec2(0,1) );
    vec4 ofd = hash4( iuv + ivec2(1,1) );

    vec2 ddx = dFdx( uv );
    vec2 ddy = dFdy( uv );

    // transform per-tile uvs
    ofa.zw = sign( ofa.zw-0.5 );
    ofb.zw = sign( ofb.zw-0.5 );
    ofc.zw = sign( ofc.zw-0.5 );
    ofd.zw = sign( ofd.zw-0.5 );

    // uv's, and derivarives (for correct mipmapping)
    vec2 uva = uv*ofa.zw + ofa.xy, ddxa = ddx*ofa.zw, ddya = ddy*ofa.zw;
    vec2 uvb = uv*ofb.zw + ofb.xy, ddxb = ddx*ofb.zw, ddyb = ddy*ofb.zw;
    vec2 uvc = uv*ofc.zw + ofc.xy, ddxc = ddx*ofc.zw, ddyc = ddy*ofc.zw;
    vec2 uvd = uv*ofd.zw + ofd.xy, ddxd = ddx*ofd.zw, ddyd = ddy*ofd.zw;

    // fetch and blend
    vec2 b = smoothstep( 0.25,0.75, fuv );

    return mix( mix( textureGrad( samp, uva, ddxa, ddya ),
                     textureGrad( samp, uvb, ddxb, ddyb ), b.x ),
                mix( textureGrad( samp, uvc, ddxc, ddyc ),
                     textureGrad( samp, uvd, ddxd, ddyd ), b.x), b.y );
}


// The standard Phong lighting equation
vec3 phongPointLight(in vec4 position, in vec3 norm)
{
    vec3 s;
    if (light_pos.w == 0.0) // directional light
        s = normalize(vec3(light_pos.xyz));
    else
        s = normalize(vec3(light_pos - position));
    vec3 v = normalize(-position.xyz);
    vec3 r = reflect( -s, norm );

    vec3 ambient = light_ambient * mtl_ambient;

    // The diffuse component
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = light_diffuse * mtl_diffuse * sDotN;

    // The specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = light_specular * mtl_specular *
            pow( max( dot(r,v), 0.0 ), shininess );

    //return ambient + diffuse + spec;
    return ambient + diffuse;
}

//For function
//Source: http://iquilezles.org/www/articles/fog/fog.htm
vec3 applyFog( in vec3  rgb,       // original color of the pixel
               in float distance,float b ) // camera to point distance
{
    float fogAmount = 1.0 - exp( -distance*b );
    vec3  fogColor  = vec3(0.5,0.6,0.7);
    return mix( rgb, fogColor, fogAmount );
}

void main(void)
{
    vec4 sideClr = vec4(0.8, 0.6, 0.2, 1.0);
	vec4 topClr = vec4(0.2, 0.6, 0.6, 1.0);

    vec2 uv = tex_coord*16;
    //Height Map textures
    if(mapRender==1){
        if(pos.y <=0.005){
            fragColour=textureNoTile( texSample1, uv);
        }else if(pos.y >0.009 && pos.y <=0.026 ){
            fragColour=textureNoTile( texSample2, uv);
        }else if(pos.y >0.034 && pos.y <=0.037){
            fragColour=textureNoTile( texSample3, uv);
        }else if(pos.y >0.041){
            fragColour=textureNoTile( texSample4, uv);

        }

        else if(pos.y >0.005 && pos.y <=0.009){
            alpha=(pos.y-0.005)/0.004;
            fragColour=(textureNoTile( texSample1, uv)* vec4(1.0, 1.0, 1.0, 1.0)*(1.0-alpha)
                 + textureNoTile( texSample2, uv)*vec4(1.0, 1.0, 1.0, 1.0)*(alpha));
        }

        else if(pos.y >0.026 && pos.y <=0.034){
            alpha=(pos.y-0.026)/0.008;
            fragColour=(textureNoTile( texSample2, uv)* vec4(1.0, 1.0, 1.0, 1.0)*(1.0-alpha)
                 + textureNoTile( texSample3, uv)*vec4(1.0, 1.0, 1.0, 1.0 )*(alpha));
        }else if(pos.y >0.037 && pos.y <=0.041){
            alpha=(pos.y-0.037)/0.004;
            fragColour=(textureNoTile( texSample3, uv)* vec4(1.0, 1.0, 1.0, 1.0)*(1.0-alpha)
                 + textureNoTile( texSample4, uv)*vec4(1.0, 1.0, 1.0, 1.0 )*(alpha));
        }

    }

    // fog
    const float radius = 20.0; //distance
    float dstnc = length(dist);
    float attenuation = clamp((radius - dstnc) / radius, 0.0, 1.0);
    // fog end


    if(sphereRender==1){
                fragColour = vec4(phongPointLight(vertex, normalize(normal)), 1.0) ;

            }
    if (shadow == 0)
    {
        // The bias modifies the depth comparison for consistent results
        // when depth values in each pass are nearly equal.
        // Try removing it and see what happens!
        float bias = 0.005;

        // We will assume that a fragment in shadow only receives ambient light
        //This if statement tells the program to make shadow
        if ( texture( gShadowMap, ShadowCoord.xy ).x  <  ShadowCoord.z - bias) {
            fragColour= fragColour*vec4(light_ambient * mtl_ambient, 1.0);
            if(mapRender== 1){
                //fragColour = vec4(pos.y*20,pos.y*20,pos.y*20,1.0);
                //fragColour =  vec4(0.2f,0,0.0f,1.0);
            }
            else{
                //fragColour = vec4(light_ambient * mtl_ambient, 1.0);
                //fragColour = vec4(light_ambient * mtl_ambient, 1.0);
                //fragColour =  vec4(0.0f,0.3f,0.0f,1.0);
            }
            //fragColour = vec4(pos.x,pos.y,pos.z,1.0);
        }
        //Otherwise render the rest of the object
        else {
            if(sphereRender==1){
                fragColour = vec4(phongPointLight(vertex, normalize(normal)), 1.0) ;

            }
            if(mapRender==1){

                //fragColour = vec4(pos.y*20,pos.y*20,pos.y*20,1.0);

                //fragColour.a = 1.0f;
            }//else{
                //fragColour = vec4(phongPointLight(vertex, normalize(normal)), 1.0) * attenuation + (.5*(1.0-attenuation));
            //}
        }
        if(glpos.z> 50){
             fragColour = vec4(applyFog(vec3(fragColour.x,fragColour.y,fragColour.z),glpos.z-50,0.015),1.0f);
        }else{
             //fragColour = fragColour;
        }
    }
    else
    {
        //fragmentdepth = gl_FragCoord.z;
        //fragColour = vec4(1,0,1,1.0);
    }

}
