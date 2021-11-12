#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// material parameters
uniform sampler2D gPosition;
uniform sampler2D gNormalId;
uniform sampler2D gDepth;
uniform sampler2D gAlbedo;
uniform sampler2D gAoMetallicRoughness;

uniform sampler2D gFragPosDirLightSpace;
uniform sampler2D gFragPosPointLightSpace0;
uniform sampler2D gFragPosPointLightSpace1;
uniform sampler2D gFragPosPointLightSpace2;
uniform sampler2D gFragPosPointLightSpace3;

// lights
struct DirectionalLight {
    vec3 color;
    vec3 direction;
    sampler2D shadow_map;
};

struct PointLight {
    vec3 color;
    vec3 position;
    sampler2D shadow_map;
};

struct SpotLight {
    vec3 color;
    vec3 position, direction;
    float cut_off, outer_cut_off;
    sampler2D shadow_map;
};

uniform DirectionalLight directional_light;
uniform PointLight point_light[4];
uniform SpotLight spot_light;

uniform vec3 camera_pos;
uniform mat4 uWorldToScreen;

const float PI = 3.14159265359;

vec3 Project(vec4 a) {
    a = a / a.w;
    return a.xyz;
}

float CalVis(vec3 N, vec3 L, vec4 fragPosLightSpace, sampler2D shadow_map){
    // perform perspective divide
    vec3 projCoords = Project(fragPosLightSpace);
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)

    float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (projCoords.z > 1.0){
        shadow = 0.0;
    }

    return 1.0-shadow;
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 CalBRDF(vec3 N, vec3 V, vec3 L, vec3 F0, float roughness, float metallic, vec3 albedo) {
    vec3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;// + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    return (kD * albedo / PI + specular)  * NdotL;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}

vec3 Shading(vec2 uv){
    vec3 albedo = pow(texture(gAlbedo, uv).rgb, vec3(2.2));
    //    float metallic  = texture(metallic_map, uv).r;
    float metallic = 0.f;
    //    float roughness = texture(gAlbedoRoughness, uv).r;
    float roughness = 0.9f;

    //    vec3 N = getNormalFromMap();
    vec3 N = texture(gNormalId, uv).rgb;
    vec3 WorldPos = Project(texture(gPosition, uv));
    vec3 V = normalize(camera_pos - WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(point_light[i].position - WorldPos);

        float distance = length(point_light[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = point_light[i].color * attenuation;

        vec4 fragPosLightSpace;
        if (i==0){
            fragPosLightSpace = texture(gFragPosPointLightSpace0, uv);
        } else if (i==1){
            fragPosLightSpace = texture(gFragPosPointLightSpace1, uv);
        } else if (i==2){
            fragPosLightSpace = texture(gFragPosPointLightSpace2, uv);
        } else if (i==3){
            fragPosLightSpace = texture(gFragPosPointLightSpace3, uv);
        }
        Lo += CalVis(N, L, fragPosLightSpace, point_light[i].shadow_map) * CalBRDF(N, V, L, F0, roughness, metallic, albedo) * radiance;
    }

    // calculate direction light radiance
    vec3 L = -normalize(directional_light.direction);
    vec3 radiance = directional_light.color;

    vec4 fragPosLightSpace = texture(gFragPosDirLightSpace, uv);
    Lo += CalVis(N, L, fragPosLightSpace, directional_light.shadow_map) * CalBRDF(N, V, L, F0, roughness, metallic, albedo) * radiance;

    // calculate spot light radiance
    L = normalize(spot_light.position - WorldPos);
    float distance = length(spot_light.position - WorldPos);
    float attenuation = 1.0 / (distance * distance);
    float theta = dot(L, normalize(-spot_light.direction));
    float epsilon = spot_light.cut_off - spot_light.outer_cut_off;
    float intensity = clamp((theta - spot_light.outer_cut_off) / epsilon, 0.0, 1.0);
    radiance = spot_light.color * attenuation * intensity;

    Lo += CalBRDF(N, V, L, F0, roughness, metallic, albedo) * radiance;

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    //    float ao = 1.f;
    float ao = texture(gAoMetallicRoughness, uv).r;
    vec3 ambient = vec3(0.03) * albedo * ao;

    return ambient + Lo;
}

vec2 GetScreenCoordinate(vec3 posWorld) {
    vec2 uv = Project(uWorldToScreen * vec4(posWorld, 1.0)).xy * 0.5 + 0.5;
    return uv;
}

float GetDepth(vec3 posWorld) {
    float depth = Project(uWorldToScreen * vec4(posWorld, 1.0)).z * 0.5 + 0.5;
    return depth;
}

float GetGBufferDepth(vec2 uv) {
    float depth = texture2D(gDepth, uv).x;
    if (depth < 1e-2) {
        depth = 1000.0;
    }
    return depth;
}

bool RayMarch(vec3 ori, vec3 dir, out vec3 hit_pos) {
    float step = 1e-2;
    float level = 0.0;
    vec3 next_point = ori;
    vec2 uv;
    for (int i=0;i<10000;i++){
        vec3 old_point = next_point;
        next_point += step*pow(2.0, level)*dir;
        uv = GetScreenCoordinate(next_point);
        if ((GetDepth(next_point) > GetGBufferDepth(uv)) || (uv.x>1.0 || uv.x<0.0 || uv.y>1.0 || uv.y<0.0)){
            next_point = old_point;
            uv = GetScreenCoordinate(next_point);
            level -= 1.0;
        } else {
            level += 1.0;
        }
        if (level < 0.0){
            break;
        }
    }
    if (GetGBufferDepth(uv)!=1.0 && abs(GetDepth(next_point)-GetGBufferDepth(uv)) < step){
        hit_pos = next_point;
        return true;
    }
    return false;
}

void main() {
    vec3 pos = Project(texture(gPosition, TexCoords));
    vec2 uv = GetScreenCoordinate(pos);
    float id = texture(gNormalId, TexCoords).w;
    vec3 color = vec3(0.0);
    if (0.05 < id && id < 0.25){
        color = Shading(uv);
    } else if (0.25<id && id<0.35){
        vec3 N = normalize(texture(gNormalId, uv).rgb);
        vec3 V = normalize(camera_pos - pos);
        vec3 dir = reflect(-V, N);
        vec3 hit_pos;
        if (RayMarch(pos, dir, hit_pos)){
            color = Shading(GetScreenCoordinate(hit_pos));
        } else {
            discard;
        }
    }

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
