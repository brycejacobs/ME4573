uniform sampler2D image;
varying vec4 projCoord;
void main() {
		gl_FragColor = texture2DProj(image, gl_TexCoord[0].xyzw);
	

}