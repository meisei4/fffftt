// DIRECT COPY OF https://www.shadertoy.com/view/tcG3Rm source
// Fork of "Visualiser 203" by Cotterzz. https://shadertoy.com/view/tsGGRy
// 2025-05-22 11:51:47

// If you want to change tune, click on the iChannel0 audio display below, then on music tab and select different track
//                                                              Electronebulae and X'TrackTure are both good.

#define TWO_PI 6.28318530718

#define C9 8372.02
#define Db9 8870.
#define D9 9398.
#define Eb9 9974.
#define E9 10548.
#define F9 11176.
#define Gb9 11840.
#define G9 12544.
#define Ab9 13290.
#define A9 14080.
#define Bb9 14918.
#define B9 15804.

float notes[] = float[](C9, Db9, D9,Eb9,E9,F9,Gb9,G9,Ab9,A9,Bb9,B9);

float flats[] = float[](1., 0.5, 1., 0.5, 1.,1.,0.5, 1., 0.5, 1., 0.5, 1.);

vec3 cols[] = vec3[](vec3(1,1,0),      vec3(0.5,1,0.1),vec3(0,0.8,0),  vec3(0,.8,0.6),vec3(0,0.8,1), vec3(0,0.2,1),
                     vec3(0.35,0,0.75),vec3(0.5,0,1),  vec3(0.8,0,0.7),vec3(0.8,0,0), vec3(1,0.1,0), vec3(1,0.5,0));

void mainImage( out vec4 O, vec2 u )
{
    vec2 U = u / iResolution.xy,
        to = U * vec2(12,10),
        TO = floor(to),
        D  =  abs(fract(to) - .5);
       vec3 col = cols[int(TO.x)]; float flt = flats[int(TO.x)];
       
    float dist = max(D.x, D.y),
     frequency = notes[int(TO.x)] / exp2(9.-TO.y),
        f = texture(iChannel0, vec2(frequency/11025.,0)).r,
        fa = texture(iChannel0, vec2((frequency*1.13)/11025.,0)).r,
        fb = texture(iChannel0, vec2((frequency*0.87)/11025.,0)).r,
        fc = texture(iChannel0, vec2((frequency*1.2)/11025.,0)).r,
        fd = texture(iChannel0, vec2((frequency*0.8)/11025.,0)).r;
        float av = (f+fa+fb+fc+fd)/5.;
        float ff = (f-av)*8.*f;
        f=(f+f+f+ff+ff)/4.;
		float bright = smoothstep(.0, .01 , (f*f*f*f)-dist*2. );
	
    O = bright * vec4(col*flt, 1);
}