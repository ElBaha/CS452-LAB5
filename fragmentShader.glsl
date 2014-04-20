#version 150

uniform float triangleStrobe;
uniform sampler2D tex;
out vec4 finalColor;
in vec3 vertexColor;

in vec2 Texcoord;

void main(){
	finalColor=texture(tex, Texcoord)*(vec4(vertexColor, 1.0)*triangleStrobe);
}
