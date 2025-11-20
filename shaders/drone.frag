// Alien Scout Drone Fragment Shader
// Advanced alien tech rendering with holographic effects
#version 120

//  Input from vertex shader
varying vec3 View;
varying vec3 Norm;
varying vec3 WorldPos;
varying vec4 Kd;
varying vec2 TexCoord;
varying float DistFromCenter;

//  Uniforms
uniform float time;
uniform float energyLevel;
uniform int droneMode;  // 0=idle, 1=scanning, 2=combat
uniform vec3 scanColor;

//  Constants
const vec3 lightDir = vec3(-0.5773, 0.5773, -0.5773);

//  Alien tech color palette
vec3 getTechColor(float t, int mode)
{
   if (mode == 0) {
      // Idle - cool blues and cyans
      if (t < 0.5)
         return mix(vec3(0.0, 0.2, 0.4), vec3(0.0, 0.5, 1.0), t * 2.0);
      else
         return mix(vec3(0.0, 0.5, 1.0), vec3(0.3, 0.8, 1.0), (t - 0.5) * 2.0);
   } else if (mode == 1) {
      // Scanning - cyan to green
      if (t < 0.5)
         return mix(vec3(0.0, 0.8, 1.0), vec3(0.0, 1.0, 0.8), t * 2.0);
      else
         return mix(vec3(0.0, 1.0, 0.8), vec3(0.5, 1.0, 0.5), (t - 0.5) * 2.0);
   } else {
      // Combat - red to orange
      if (t < 0.5)
         return mix(vec3(1.0, 0.0, 0.0), vec3(1.0, 0.3, 0.0), t * 2.0);
      else
         return mix(vec3(1.0, 0.3, 0.0), vec3(1.0, 0.7, 0.0), (t - 0.5) * 2.0);
   }
}

//  Holographic scan lines
float scanLines(vec2 uv, float time)
{
   float lines = sin(uv.y * 50.0 + time * 5.0) * 0.5 + 0.5;
   lines = pow(lines, 3.0);
   return lines * 0.3;
}

//  Hexagonal tech pattern
float hexPattern(vec2 uv)
{
   vec2 p = uv * 8.0;
   vec2 h = vec2(1.0, 1.732);
   vec2 grid = mod(p, h) - h * 0.5;
   float d = length(grid);
   return smoothstep(0.3, 0.35, d);
}

//  Energy field shimmer
float energyShimmer(vec3 pos, float time)
{
   float shimmer = 0.0;
   shimmer += sin(pos.x * 10.0 + time * 2.0) * 0.5;
   shimmer += sin(pos.y * 15.0 - time * 3.0) * 0.3;
   shimmer += sin(pos.z * 12.0 + time * 1.5) * 0.4;
   return (shimmer + 1.2) * 0.5;
}

//  Fresnel effect for energy shield
float fresnel(vec3 normal, vec3 viewDir, float power)
{
   float dotNV = max(dot(normal, viewDir), 0.0);
   return pow(1.0 - dotNV, power);
}

//  Procedural circuit board pattern
float circuitPattern(vec2 uv, float time)
{
   vec2 p = uv * 20.0;
   
   // Horizontal lines
   float h = step(0.9, fract(p.y));
   
   // Vertical lines
   float v = step(0.9, fract(p.x));
   
   // Connection nodes
   float nodes = 0.0;
   vec2 nodePos = floor(p);
   float nodeHash = fract(sin(dot(nodePos, vec2(12.9898, 78.233))) * 43758.5453);
   if (nodeHash > 0.7) {
      vec2 localPos = fract(p);
      nodes = 1.0 - smoothstep(0.0, 0.3, length(localPos - 0.5));
   }
   
   // Animated pulse through circuits
   float pulse = sin(p.x * 0.5 + time * 3.0) * 0.5 + 0.5;
   
   return max(max(h, v) * pulse, nodes);
}

void main()
{
   //  Normalize inputs
   vec3 normal = normalize(Norm);
   vec3 viewDir = normalize(View);
   
   //  Base material color (dark metallic)
   vec3 baseColor = vec3(0.08, 0.08, 0.12);
   
   //  Tech color based on mode and time
   float colorPhase = mod(time * 0.3 + DistFromCenter * 0.2, 1.0);
   vec3 techColor = getTechColor(colorPhase, droneMode);
   
   //  Holographic scan lines
   float scanEffect = scanLines(TexCoord, time);
   
   //  Hexagonal panel pattern
   float hexPattern = hexPattern(TexCoord);
   
   //  Circuit board pattern for tech panels
   float circuits = circuitPattern(TexCoord, time);
   
   //  Energy shimmer across surface
   float shimmer = energyShimmer(WorldPos, time);
   
   //  Fresnel for energy shield effect
   float fresnelTerm = fresnel(normal, viewDir, 3.0);
   
   //  Combine patterns
   vec3 panelColor = baseColor;
   panelColor = mix(panelColor, techColor * 0.3, circuits * energyLevel);
   panelColor = mix(panelColor, techColor * 0.5, hexPattern * 0.3);
   panelColor += techColor * scanEffect * energyLevel;
   
   //  Energy field glow
   vec3 energyGlow = techColor * fresnelTerm * energyLevel * 0.8;
   panelColor += energyGlow;
   
   //  Surface shimmer
   panelColor *= (0.8 + shimmer * 0.4 * energyLevel);
   
   //  Lighting
   float diffuse = max(dot(normal, lightDir), 0.0);
   vec3 halfDir = normalize(lightDir + viewDir);
   float specular = pow(max(dot(normal, halfDir), 0.0), 64.0);
   
   //  Combine lighting
   vec3 finalColor = panelColor * (0.4 + diffuse * 0.6);
   finalColor += techColor * specular * 0.8;
   
   //  Apply vertex color modulation
   finalColor *= Kd.rgb;
   
   //  Rim lighting for holographic effect
   float rim = pow(1.0 - max(dot(normal, viewDir), 0.0), 4.0);
   finalColor += techColor * rim * energyLevel * 1.2;
   
   //  Pulsing core glow
   float corePulse = sin(time * 6.0) * 0.5 + 0.5;
   finalColor += techColor * corePulse * (1.0 - DistFromCenter) * 0.3;
   
   //  Output
   gl_FragColor = vec4(finalColor, Kd.a);
}
