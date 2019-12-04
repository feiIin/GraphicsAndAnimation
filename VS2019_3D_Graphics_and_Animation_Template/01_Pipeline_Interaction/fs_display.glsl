#version 450 core
// Image processing kernel from here:
// http://haishibai.blogspot.com/2009/09/image-processing-c-tutorial-4-gaussian.html

in vec2 TexCoords;
out vec4 color;

uniform float width;			// Screen width
uniform float height;			// Screen Height
uniform sampler2D screenTexture;

void main()
{
	// Total value of RGB
	vec3 total = vec3(0.0);

	// Blur amount in pixels
	float blurAmount = 2.0;

	// Amount of blur - change this value for more or less;
	float blur = blurAmount/width;

	// Blur in x
	float hstep = 1.0; float vstep = 0.0;
	total += texture(screenTexture, vec2(TexCoords.x - 4.0*blur*hstep, TexCoords.y - 4.0*blur*vstep)).rgb * 0.0162162162;
	total += texture(screenTexture, vec2(TexCoords.x - 3.0*blur*hstep, TexCoords.y - 3.0*blur*vstep)).rgb * 0.0540540541;
	total += texture(screenTexture, vec2(TexCoords.x - 2.0*blur*hstep, TexCoords.y - 2.0*blur*vstep)).rgb * 0.1216216216;
	total += texture(screenTexture, vec2(TexCoords.x - 1.0*blur*hstep, TexCoords.y - 1.0*blur*vstep)).rgb * 0.1945945946;
	total += texture(screenTexture, TexCoords).rgb * 0.2270270270;
	total += texture(screenTexture, vec2(TexCoords.x + 1.0*blur*hstep, TexCoords.y + 1.0*blur*vstep)).rgb * 0.1945945946;
	total += texture(screenTexture, vec2(TexCoords.x + 2.0*blur*hstep, TexCoords.y + 2.0*blur*vstep)).rgb * 0.1216216216;
	total += texture(screenTexture, vec2(TexCoords.x + 3.0*blur*hstep, TexCoords.y + 3.0*blur*vstep)).rgb * 0.0540540541;
	total += texture(screenTexture, vec2(TexCoords.x + 4.0*blur*hstep, TexCoords.y + 4.0*blur*vstep)).rgb * 0.0162162162;

	// Blur in y
	 hstep = 0.0;  vstep = 1.0;
	total += texture(screenTexture, vec2(TexCoords.x - 4.0*blur*hstep, TexCoords.y - 4.0*blur*vstep)).rgb * 0.0162162162;
	total += texture(screenTexture, vec2(TexCoords.x - 3.0*blur*hstep, TexCoords.y - 3.0*blur*vstep)).rgb * 0.0540540541;
	total += texture(screenTexture, vec2(TexCoords.x - 2.0*blur*hstep, TexCoords.y - 2.0*blur*vstep)).rgb * 0.1216216216;
	total += texture(screenTexture, vec2(TexCoords.x - 1.0*blur*hstep, TexCoords.y - 1.0*blur*vstep)).rgb * 0.1945945946;
	total += texture(screenTexture, TexCoords).rgb * 0.2270270270;
	total += texture(screenTexture, vec2(TexCoords.x + 1.0*blur*hstep, TexCoords.y + 1.0*blur*vstep)).rgb * 0.1945945946;
	total += texture(screenTexture, vec2(TexCoords.x + 2.0*blur*hstep, TexCoords.y + 2.0*blur*vstep)).rgb * 0.1216216216;
	total += texture(screenTexture, vec2(TexCoords.x + 3.0*blur*hstep, TexCoords.y + 3.0*blur*vstep)).rgb * 0.0540540541;
	total += texture(screenTexture, vec2(TexCoords.x + 4.0*blur*hstep, TexCoords.y + 4.0*blur*vstep)).rgb * 0.0162162162;

	//color = vec4(total/2.0, 1.0);

	// Simple example to make everything grayscale...
	//float average = (color.r + color.g + color.b) / 3.0;
    //color = vec4(average, average, average, 1.0);

	// for no blur
	color = vec4(texture(screenTexture, vec2(TexCoords.x, TexCoords.y)).rgb, 1.0) ; 
}
