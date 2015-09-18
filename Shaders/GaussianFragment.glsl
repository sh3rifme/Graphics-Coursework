#version 330 core

// Performs 5x5 matrix Gaussian blur via 2 passes (vertical/horizontal as determined by isVertical)

uniform sampler2D diffuseTex; // our generated texture to post process
uniform vec2 pixelSize;
uniform int isVertical; // controls whether blurring vertically/horizontally

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;

// Technically this isn't quite a Gaussian distribution
const float weights[5] = float[](0.12, 0.22, 0.32, 0.22, 0.12); // sum(weights) == 1, maintains overall brightness following blurring

void main(void) {
	vec2 values[5];

	if (isVertical == 1) { // Vertical pass
/*		values = vec2[](vec2(0.0, -pixelSize.y * 3), 
						vec2(0.0, -pixelSize.y * 2),
						vec2(0.0, pixelSize.y),
						vec2(0.0, pixelSize.y * 2),
						vec2(0.0, pixelSize.y * 3));
	*/
	values = vec2[](vec2(0.0, -pixelSize.y * 2), 
						vec2(0.0, -pixelSize.y),
						vec2(0.0, 0.0),
						vec2(0.0, pixelSize.y),
						vec2(0.0, pixelSize.y * 2));
	}
	else {
/*		values = vec2[](vec2(-pixelSize.x * 3, 0.0), 
						vec2(-pixelSize.x * 2, 0.0), 
						vec2(pixelSize.x, 0.0),
						vec2(pixelSize.x * 2, 0.0),
						vec2(pixelSize.x * 3, 0.0));*/

		values = vec2[](vec2(-pixelSize.x * 2, 0.0), 
						vec2(-pixelSize.x , 0.0), 
						vec2(0.0, 0.0),
						vec2(pixelSize.x, 0.0),
						vec2(pixelSize.x * 2, 0.0));
	}

	for (int i = 0; i < 5; ++i) {
		vec4 tmp = texture2D(diffuseTex, IN.texCoord + values[i]);
		gl_FragColor += tmp * weights[i];
	}
}