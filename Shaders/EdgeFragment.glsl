#version 150 core

uniform sampler2D diffuseTex;
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
	for (int i = 0; i < 9; i++ ) {
		vec3 tmp = texture2D(diffuseTex, IN.texCoord.xy + values[i]).rgb;
		outvec +=  (tmp * weightsX[i]) + (tmp * weightsY[i]);
	}
	
	bvec3 stepVec = lessThan(vec3(-0.5,-0.5,-0.5), outvec);
	
	if (stepVec.x && stepVec.y && stepVec.z) {
		gl_FragColor = texture2D(diffuseTex, IN.texCoord);
	}
	else {
		gl_FragColor = vec4(0.0,0.0,0.0,1.0);
	}
}