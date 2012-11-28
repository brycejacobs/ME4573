varying vec4 projCoord;
void main() {
	gl_Position = gl_ModelViewMatrix * gl_Vertex;
	vec4 pos = gl_ModelViewMatrix * gl_Vertex;
	projCoord = gl_TextureMatrix[4] * pos;


}