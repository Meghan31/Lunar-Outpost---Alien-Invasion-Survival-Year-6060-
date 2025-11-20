// Crystal Fragment Shader
// Advanced rendering with Fresnel effect, chromatic aberration, and iridescence
#version 120

//  Input from vertex shader
varying vec3 View;
varying vec3 Norm;
varying vec3 WorldPos;
varying vec4 Kd;
varying vec2 TexCoord;

//  Uniforms
uniform float time;
uniform vec3 cameraPos;
uniform float transparency;
uniform float iridescence;
uniform int enableChromatic;

//  Constants
const float IOR = 1.5; // Index of refraction for crystal
const vec3 lightDir = vec3(-0.5773, 0.5773, -0.5773); // Normalized (-1, 1, -1)

// Fresnel effect (Schlick's approximation)
float fresnelSchlick(float cosTheta, float F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Convert spectral wavelength to RGB (alien color palette)
vec3 spectralColor(float t)
{
    // Enhanced spectral gradient
    vec3 color;
    
    if (t < 0.25) {
        // Deep cyan to electric blue
        float local = t * 4.0;
        color = vec3(0.05 + local * 0.15, 0.85 - local * 0.35, 1.0);
    } else if (t < 0.5) {
        // Electric blue to violet
        float local = (t - 0.25) * 4.0;
        color = vec3(0.2 + local * 0.6, 0.5 - local * 0.4, 1.0);
    } else if (t < 0.75) {
        // Violet to magenta
        float local = (t - 0.5) * 4.0;
        color = vec3(0.8 + local * 0.2, 0.1 + local * 0.1, 1.0 - local * 0.1);
    } else {
        // Magenta to hot pink
        float local = (t - 0.75) * 4.0;
        color = vec3(1.0, 0.2 + local * 0.3, 0.9 - local * 0.4);
    }
    
    return color;
}

// Chromatic aberration simulation
vec3 chromaticAberration(vec3 normal, vec3 viewDir)
{
    // Simulate RGB refraction at different angles
    float dotNV = abs(dot(normal, viewDir));
    
    // Different refraction for each channel
    float r_refract = 1.0 + (1.0 - dotNV) * 0.05;
    float g_refract = 1.0 + (1.0 - dotNV) * 0.03;
    float b_refract = 1.0 + (1.0 - dotNV) * 0.01;
    
    vec3 color;
    color.r = spectralColor(mod(time * 0.1 + r_refract, 1.0)).r;
    color.g = spectralColor(mod(time * 0.1 + g_refract, 1.0)).g;
    color.b = spectralColor(mod(time * 0.1 + b_refract, 1.0)).b;
    
    return color;
}

// Iridescent shimmer effect
float iridescenceEffect(vec3 normal, vec3 viewDir, float time)
{
    // Angle-dependent color shift
    float angle = dot(normal, viewDir);
    float shimmer = sin(angle * 10.0 + time * 2.0) * 0.5 + 0.5;
    
    // Add noise-like variation
    float noise = fract(sin(dot(WorldPos.xy, vec2(12.9898, 78.233))) * 43758.5453);
    shimmer = mix(shimmer, noise, 0.2);
    
    return shimmer;
}

void main()
{
    //  Normalize inputs
    vec3 normal = normalize(Norm);
    vec3 viewDir = normalize(View);
    
    //  Calculate Fresnel term
    float dotNV = max(dot(normal, viewDir), 0.0);
    float fresnel = fresnelSchlick(dotNV, 0.04);
    
    //  Base spectral color with animation
    float colorPhase = mod(time * 0.08 + length(WorldPos) * 0.1, 1.0);
    vec3 baseColor = spectralColor(colorPhase);
    
    //  Apply chromatic aberration if enabled
    vec3 refractionColor = baseColor;
    if (enableChromatic == 1) {
        refractionColor = chromaticAberration(normal, viewDir);
    }
    
    //  Iridescence effect
    float iridShimmer = iridescenceEffect(normal, viewDir, time);
    vec3 iridColor = spectralColor(colorPhase + iridShimmer * 0.3);
    
    //  Mix base color with iridescence
    vec3 finalColor = mix(refractionColor, iridColor, iridescence * iridShimmer);
    
    //  Apply Fresnel - more reflective at grazing angles
    finalColor = mix(finalColor, vec3(1.0, 0.95, 1.0), fresnel * 0.5);
    
    //  Basic lighting
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 halfDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(normal, halfDir), 0.0), 128.0);
    
    //  Combine lighting
    finalColor = finalColor * (0.3 + diffuse * 0.7) + vec3(specular * 0.5);
    
    //  Apply vertex color tint
    finalColor *= Kd.rgb;
    
    //  Rim lighting for edge glow
    float rim = pow(1.0 - dotNV, 3.0);
    finalColor += spectralColor(colorPhase + 0.5) * rim * 0.8;
    
    //  Final alpha with transparency
    float alpha = mix(Kd.a, Kd.a * (1.0 - fresnel), transparency);
    
    //  Output with HDR-like boost
    gl_FragColor = vec4(finalColor * 1.2, alpha);
}
