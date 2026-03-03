varying vec3 vNormal;
varying vec3 vPos;
varying vec2 vTexCoord;

void main()
{
    vNormal     = normalize(gl_NormalMatrix * gl_Normal);
    vPos        = vec3(gl_ModelViewMatrix * gl_Vertex);
    vTexCoord   = gl_MultiTexCoord0.xy;
    gl_Position = ftransform();
}