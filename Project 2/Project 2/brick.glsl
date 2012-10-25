varying vec3 var_L;
varying vec3 var_N;
varying vec2 var_P;
void main()
{
vec3 L = normalize(var_L); // Light vector
vec3 N = normalize(var_N); // Normal vector
float kd = max(dot(N, L), 0.0); // Diffuse intensity
gl_FragColor = vec4(material_color(var_P) * kd, 1.0); // Fragment color
}