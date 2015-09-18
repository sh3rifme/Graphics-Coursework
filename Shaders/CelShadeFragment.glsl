#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform vec2 pixelSize;

in Vertex {
	vec2 texCoord ;
	vec4 colour ;
} IN ;

out vec4 gl_FragColor ;
const float weightsX[9] = float [](-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0, 1.0);
const float weightsY[9] = float [](1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);

void main (void) {
	vec2 values [9];

	values = vec2[](vec2(-pixelSize.x , pixelSize.y), vec2(0.0, pixelSize.y),  vec2(pixelSize.x, pixelSize.y),
						  vec2(-pixelSize.x, 0.0), 			 vec2(0.0, 0.0),			 vec2(pixelSize.x, 0.0), 
						  vec2(-pixelSize.x, -pixelSize.y), vec2(0.0, -pixelSize.y), vec2(pixelSize.x, -pixelSize.y));

	vec3 outvec;
	
	
	float f = 10000.0;
	float n = 1.0;
	float z;// = (2 * n) / (f + n - texture2D(diffuseTex, IN.texCoord).x * (f - n));
	
	for (int i = 0; i < 9; i++ ) {
		vec3 tmp = texture2D(depthTex, IN.texCoord.xy + values[i]).rgb;
		z = (2 * n) / (f + n - tmp.x * (f - n));	
		outvec +=  vec3(z * weightsX[i]) + vec3(z * weightsY[i]);
	}
	
	//gl_FragColor = vec4(outvec, 1.0);
	
	float c = 0.05;
	
	bvec3 stepVec = greaterThan(vec3(c,c,c), outvec);
	
	if (stepVec.x && stepVec.y && stepVec.z) {
		gl_FragColor = texture2D(diffuseTex, IN.texCoord);
	}
	else {
		gl_FragColor = vec4(0.0,0.0,0.0,1.0);
	}
}