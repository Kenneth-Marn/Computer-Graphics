
#version 330

in vec4 pos; // Interpolated vertex position, in object coords
in vec4 colour;
in vec2 Texcoord;
// The final colour we will see at this location on screen
out vec4 fragColour;

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


void main(void)
{
    vec2 uv = Texcoord*16;

	// We will ignore vertex colour this time
	// Draw the top of the cube a different colour to the rest
        //color = texture(texture1, texCoord) * vec4(1.0f, 1.0f, 1.0f, partOf1) + texture(texture2, texCoord) * vec4(1.0f, 1.0f, 1.0f, 1.0f - partOf1);

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

        /*else{
             fragColour=(textureNoTile( texSample2, uv)* vec4(1.0, 1.0, 1.0, partOf1)
                 + textureNoTile( texSample3, uv)*vec4(1.0, 1.0, 1.0, 1.0 - partOf1));
        }*/
        //fragColour =  texture(texSample, Texcoord);

        //fragColour = vec4(0.0,0.0,0.0,1.0);
}
