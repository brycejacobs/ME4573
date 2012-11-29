
void main() {
	vec4 transformed_vert = ftransform();
	gl_TexCoord[0] = transformed_vert;
	gl_Position = transformed_vert;
}