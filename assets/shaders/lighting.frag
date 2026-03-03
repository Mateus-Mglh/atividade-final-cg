uniform sampler2D uTexture;
uniform vec3 uAmuletPos[3];
uniform int uAmuletCount;
uniform bool uUseTexture;
uniform vec3 uSunDir;       // ← novo
uniform vec3 uSunColor;     // ← novo
uniform float uSunIntensity; // ← novo
uniform vec3 uAmbient;

varying vec3 vNormal;
varying vec3 vPos;
varying vec2 vTexCoord;

void main()
{
    vec3 lightDir = normalize(uSunDir);
    float diff    = max(dot(vNormal, lightDir), 0.0);

    vec3 ambient = uAmbient;
    vec3 diffuse = uSunColor * diff * uSunIntensity;

    vec3 amuletLight = vec3(0.0);
    for (int i = 0; i < uAmuletCount; i++)
    {
        vec3 toAmulet = uAmuletPos[i] - vPos;
        float dist    = length(toAmulet);
        float atten   = 1.0 / (1.0 + 0.02 * dist * dist);
        float aDiff   = max(dot(vNormal, normalize(toAmulet)), 0.0);
        amuletLight  += vec3(0.8, 0.6, 0.15) * aDiff * atten;
    }

    vec3 baseColor;
    if (uUseTexture)
        baseColor = texture2D(uTexture, vTexCoord).rgb;
    else
        baseColor = vec3(0.6, 0.5, 0.4);

    vec3 lighting = ambient + diffuse + amuletLight;
    gl_FragColor  = vec4(lighting * baseColor, 1.0);
}