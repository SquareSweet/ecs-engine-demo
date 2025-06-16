#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 objectColor;
uniform sampler2D diffuseTexture;
uniform bool useTexture;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 ambient = 0.1 * lightColor;

    vec3 resultColor = ambient + diffuse;
    vec3 baseColor = objectColor;
    if (useTexture) {
        baseColor = texture(diffuseTexture, TexCoord).rgb;
    }
    resultColor *= baseColor;
    FragColor = vec4(resultColor, 1.0);
}
