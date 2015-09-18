#version 150 core
uniform sampler2D depthTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;

void main(){
vec4 color_out;
float znear = 1.0;
float zfar = 50000.0;
float depthm = texture2D(depthTex, IN.texCoord.xy).r;
float lineAmp = mix( 0.001, 0.0, clamp( (500.0 / (zfar + znear - ( 2.0 * depthm - 1.0 ) * (zfar - znear) )/2.0), 0.0, 1.0 ) );// make the lines thicker at close range

float depthn = texture2D(depthTex, IN.texCoord.xy + vec2( (0.002 + lineAmp)*0.625 , 0.0) ).r;
depthn = depthn / depthm;

float depths = texture2D(depthTex, IN.texCoord.xy - vec2( (0.002 + lineAmp)*0.625 , 0.0) ).r;
depths = depths / depthm;

float depthw = texture2D(depthTex, IN.texCoord.xy + vec2(0.0 , 0.002 + lineAmp) ).r;
depthw = depthw / depthm;

float depthe = texture2D(depthTex, IN.texCoord.xy - vec2(0.0 , 0.002 + lineAmp) ).r;
depthe = depthe / depthm;

float Contour = -4.0 + depthn + depths + depthw + depthe;

float lineAmp2 = 100.0 * clamp( depthm - 0.99, 0.0, 1.0);
lineAmp2 = lineAmp2 * lineAmp2;
Contour = (512.0 + lineAmp2 * 204800.0 ) * Contour;

if(Contour > 0.15){
    Contour = (0.15 - Contour) / 1.5 + 0.5;
} else
    Contour = 1.0;

color_out.rgb = color_out.rgb * Contour;
    color_out.a = 1.0;
    gl_FragColor = color_out;
}