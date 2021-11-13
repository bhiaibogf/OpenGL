#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D uMap;

void main() {
    vec2 texel_size = 1.0 / vec2(textureSize(uMap, 0));
    float result = 0.0;
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            vec2 offset = vec2(float(i), float(j)) * texel_size;
            result += texture(uMap, TexCoords + offset).r;
        }
    }
    FragColor = result / (5.0 * 5.0);
}
