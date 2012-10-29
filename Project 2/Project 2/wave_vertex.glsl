uniform float time;
void main()
{
vec4 P = gl_Vertex;
P.y += sin(P.x + time * 3.0) + sin(P.z + time * 2.0);
gl_FrontColor = gl_FrontMaterial.diffuse; // Vertex color
gl_Position = gl_ModelViewProjectionMatrix * P; // Vertex position
}