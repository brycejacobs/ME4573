
uniform sampler2D diffuse,normalvec, specular;
 
void main()
{
	vec4 texval1 = texture2D(diffuse, vec2(gl_TexCoord[0]));
	vec4 texval2 = texture2D(normalvec, vec2(gl_TexCoord[2]));
	vec4 texval3 = texture2D(specular, vec2(gl_TexCoord[4]));

	gl_FragColor = 0.5*(texval1 + texval2 + texval3);
}