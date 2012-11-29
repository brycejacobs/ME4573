uniform sampler2D image;

void main() {
		gl_FragColor = texture2DProj(image, gl_TexCoord[0].xyzw);

}