#version 450 core

in VERTEX_OUT{
    vec3 fragmentPosition;
    vec2 textureCoordinates;
    mat3 TBN;
    vec3 tangentViewPosition;
    vec3 tangentFragmentPosition;
} fragment_in;

struct PointLight {
    vec4 position;
    vec4 color;
    float radius;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
    PointLight lights[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
    int indices[];
} visibleLightIndicesBuffer;

// Uniforms
uniform vec3 diffuse;
uniform vec3 specular;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform uint numberOfTilesX;

out vec4 fragColor;

// Attenuate the point light intensity
float attenuate(vec3 lightDirection, float radius) {
    float cutoff = 0.5;
    float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
    attenuation = 1.0 / (attenuation * 15.0 + 1.0);
    attenuation = (attenuation - cutoff) / (1.0 - cutoff);

    return clamp(attenuation, 0.0, 1.0);
}

void main() {
    // Determine which tile this pixel belongs to
    ivec2 location = ivec2(gl_FragCoord.xy);
    ivec2 tileID = location / ivec2(16, 16);
    uint index = tileID.y * numberOfTilesX + tileID.x;

//    // Get color and normal components from texture maps
    vec4 base_diffuse = vec4(diffuse, 1.0f) * texture(diffuseMap, fragment_in.textureCoordinates);

    // Use the mask to discard any fragments that are transparent
    if (base_diffuse.a <= 0.2) {
        discard;
    }

    vec4 base_specular = vec4(specular, 1.0f) * texture(specularMap, fragment_in.textureCoordinates);
    vec3 normal = texture(normalMap, fragment_in.textureCoordinates).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 viewDirection = normalize(fragment_in.tangentViewPosition - fragment_in.tangentFragmentPosition);

    // The offset is this tile's position in the global array of valid light indices.
    // Loop through all these indices until we hit max number of lights or the end (indicated by an index of -1)
    // Calculate the lighting contribution from each visible point light
    uint offset = index * 1024;
    for (uint i = 0; i < 1024 && visibleLightIndicesBuffer.indices[offset + i] != -1; i++) {
        uint lightIndex = visibleLightIndicesBuffer.indices[offset + i];
        PointLight light = lightBuffer.lights[lightIndex];

        vec4 lightColor = light.color;
        vec3 tangentLightPosition = fragment_in.TBN * light.position.xyz;
        float lightRadius = light.radius;

//         Calculate the light attenuation on the pre-normalized lightDirection
        vec3 lightDirection = tangentLightPosition - fragment_in.tangentFragmentPosition;
        float attenuation = attenuate(lightDirection, lightRadius);

//         Normalize the light direction and calculate the halfway vector
        lightDirection = normalize(lightDirection);
        vec3 halfway = normalize(lightDirection + viewDirection);

//         Calculate the diffuse and specular components of the irradiance, then irradiance, and accumulate onto color
        float diffuse = max(dot(lightDirection, normal), 0.0);
//         How do I change the material propery for the spec exponent? is it the alpha of the spec texture?
        float specular = pow(max(dot(normal, halfway), 0.0), 32.0);

//         Hacky fix to handle issue where specular light still effects scene once point light has passed into an object
        if (diffuse == 0.0) {
            specular = 0.0;
        }

        vec3 irradiance = lightColor.rgb * ((base_diffuse.rgb * diffuse) + (base_specular.rgb * vec3(specular))) * attenuation;
        color.rgb += irradiance;
    }

    color.rgb += base_diffuse.rgb * 0.08;



    fragColor = color;
}