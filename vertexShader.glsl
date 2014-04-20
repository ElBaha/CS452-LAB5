 #version 150

in vec2 position;
in vec3 startColor;
out vec3 vertexColor;
in vec2 texcoord;
out vec2 Texcoord;
void main(){
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	vertexColor = startColor;
	Texcoord = texcoord;
}
