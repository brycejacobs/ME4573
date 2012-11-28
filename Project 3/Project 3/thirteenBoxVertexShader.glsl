
 
void main()
{
    gl_TexCoord[0] = glMultiTexCoord0;
	gl_TexCoord[2] = glMultiTexCoord2;
	gl_TexCoord[4] = glMultiTexCoord4;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}