#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform mat4 uView;
uniform mat4 uProjection;

uniform sampler2D gPosition;
uniform sampler2D gNormalId;
uniform sampler2D uNoiseMap;

uniform vec3 uSamples[64];

// parameters
int KernelSize = 64;
float Radius = 1;
float Bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 NoiseScale = vec2(800.0/5.0, 600.0/5.0);

void main() {
    float id = texture(gNormalId, TexCoords).w;
    if (id == 1.0){
        discard;
    }

    // get input for SSAO algorithm
    vec3 frag_pos = (uView * texture(gPosition, TexCoords)).xyz;

    vec3 normal = texture(gNormalId, TexCoords).xyz;
    normal = normalize(transpose(inverse(mat3(uView))) * normal);

    vec3 random_vec = normalize(texture(uNoiseMap, TexCoords * NoiseScale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for (int i = 0; i < KernelSize; i++) {
        // get sample position
        vec3 sample_pos = TBN * uSamples[i];// from tangent to view-space
        sample_pos = frag_pos + sample_pos * Radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample_pos, 1.0);
        offset = uProjection * offset;// from view to clip-space
        offset.xyz /= offset.w;// perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5;// transform to range 0.0 - 1.0

        // get sample depth
        float sample_depth = (uView * texture(gPosition, offset.xy)).z;// get depth value of kernel sample

        // range check & accumulate
        float range_check = smoothstep(0.0, 1.0, Radius / abs(frag_pos.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + Bias ? 1.0 : 0.0) * range_check;
    }
    occlusion = 1.0 - (occlusion / KernelSize);

    FragColor = occlusion;
}
