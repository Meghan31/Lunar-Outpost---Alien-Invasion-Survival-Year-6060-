// Lunaris Vex Fragment Shader
// Biomechanical alien with subsurface scattering and procedural chitin
#version 120

//  Input from vertex shader
varying vec3 View;
varying vec3 Norm;
varying vec3 WorldPos;
varying vec4 Kd;
varying vec2 TexCoord;
varying float VertexDepth;

//  Uniforms
uniform float time;
uniform float glowIntensity;
uniform int emotion;
uniform vec3 fissureColor;

//  Constants
const vec3 lightDir = vec3(-0.5773, 0.5773, -0.5773);

//  Biomechanical chitin pattern (segmented exoskeleton)
float chitinPattern(vec2 uv, float time)
{
   vec2 p = uv * vec2(8.0, 20.0);  // Vertical segments
   
   // Segment lines
   float segments = abs(fract(p.y) - 0.5) * 2.0;
   segments = smoothstep(0.85, 0.95, segments);
   
   // Subtle ribbing
   float ribs = sin(p.y * 30.0 + time * 0.5) * 0.5 + 0.5;
   ribs = pow(ribs, 4.0) * 0.3;
   
   return segments + ribs;
}

//  Organic vein pattern
float veinPattern(vec2 uv, float time)
{
   vec2 p = uv * 12.0;
   
   // Branching veins using domain distortion
   float vein1 = abs(sin(p.x * 2.0 + sin(p.y * 1.5 + time * 0.2)) * 0.5);
   float vein2 = abs(sin(p.y * 3.0 + sin(p.x * 2.0 - time * 0.3)) * 0.5);
   
   float veins = min(vein1, vein2);
   veins = smoothstep(0.1, 0.0, veins);
   
   return veins * 0.4;
}

//  Subsurface scattering approximation
vec3 subsurfaceScattering(vec3 normal, vec3 viewDir, vec3 baseColor)
{
   //  Light penetration through thin organic tissue
   float dotNL = dot(normal, lightDir);
   float backLight = max(0.0, -dotNL);
   
   //  Translucency - enhanced for organic look
   float translucency = pow(backLight, 1.5) * 0.8;
   
   //  SSS color shift (warmer, more organic/fleshy)
   vec3 sssColor = baseColor * vec3(1.2, 1.1, 1.05);
   
   //  Add slight color tint based on thickness
   vec3 thicknessTint = vec3(0.9, 0.7, 0.6);  // Warm organic tint
   sssColor = mix(sssColor, sssColor * thicknessTint, translucency * 0.5);
   
   return sssColor * translucency;
}

//  Procedural glowing fissure/crack pattern
float fissurePattern(vec2 uv, float time)
{
   //  Voronoi-based organic cracks
   vec2 p = uv * 10.0;
   vec2 ip = floor(p);
   vec2 fp = fract(p);
   
   float minDist = 1.0;
   for (int y = -1; y <= 1; y++) {
      for (int x = -1; x <= 1; x++) {
         vec2 neighbor = vec2(float(x), float(y));
         vec2 point = neighbor;
         
         //  Organic point distribution
         vec2 seed = ip + neighbor;
         point += 0.5 + 0.5 * sin(time * 0.2 + 6.2831 * fract(sin(dot(seed, vec2(127.1, 311.7))) * 43758.5453));
         
         vec2 diff = neighbor + point - fp;
         float dist = length(diff);
         minDist = min(minDist, dist);
      }
   }
   
   //  Thin glowing cracks
   float cracks = smoothstep(0.03, 0.08, minDist);
   
   //  Animated pulse along cracks
   float pulse = sin(time * 3.0 + minDist * 8.0) * 0.5 + 0.5;
   
   return (1.0 - cracks) * pulse;
}

//  Biomechanical shimmer (like insect carapace)
vec3 biomechanicalShimmer(vec3 normal, vec3 viewDir, float time)
{
   float dotNV = abs(dot(normal, viewDir));
   
   //  Angle-dependent color shift
   float shimmer = pow(1.0 - dotNV, 4.0);
   
   //  Organic color variation
   float variation = sin(time * 1.5 + dotNV * 15.0) * 0.5 + 0.5;
   
   //  Dark biomechanical palette - greys with hints of blue/purple
   vec3 color1 = vec3(0.3, 0.35, 0.4);   // Dark grey-blue
   vec3 color2 = vec3(0.45, 0.45, 0.5);  // Lighter grey
   vec3 color3 = vec3(0.35, 0.3, 0.45);  // Dark purple-grey
   
   vec3 shimmerColor = mix(color1, color2, variation);
   shimmerColor = mix(shimmerColor, color3, shimmer * 0.6);
   
   return shimmerColor * shimmer * 0.5;
}

void main()
{
   //  Normalize inputs
   vec3 normal = normalize(Norm);
   vec3 viewDir = normalize(View);
   
   //  Base biomechanical skin - dark grey with organic variation
   vec3 skinBase = vec3(0.35, 0.38, 0.42);  // Dark grey-blue base
   
   //  Add chitin pattern for exoskeleton look
   float chitin = chitinPattern(TexCoord, time);
   skinBase *= (1.0 - chitin * 0.3);  // Darken segments
   
   //  Add organic veins
   float veins = veinPattern(TexCoord, time);
   skinBase += vec3(0.1, 0.15, 0.2) * veins;  // Slightly brighter veins
   
   //  Apply biomechanical shimmer
   vec3 shimmer = biomechanicalShimmer(normal, viewDir, time);
   vec3 skinColor = skinBase + shimmer;
   
   //  Subsurface scattering for organic translucency
   vec3 sssContribution = subsurfaceScattering(normal, viewDir, skinColor);
   skinColor += sssContribution * 0.7;
   
   //  Glowing fissures (energy cracks in the exoskeleton)
   float fissures = fissurePattern(TexCoord, time);
   vec3 fissureGlow = fissureColor * fissures * glowIntensity * 2.0;
   skinColor += fissureGlow;
   
   //  Advanced lighting - wrap lighting for organic forms
   float diffuse = max(dot(normal, lightDir), 0.0);
   
   //  Wrap lighting (soft diffuse)
   float wrap = 0.6;
   float wrappedDiffuse = max(0.0, (dot(normal, lightDir) + wrap) / (1.0 + wrap));
   
   //  Specular with anisotropic highlight (like insect chitin)
   vec3 halfDir = normalize(lightDir + viewDir);
   float specular = pow(max(dot(normal, halfDir), 0.0), 40.0);
   
   //  Anisotropic specular along body segments
   vec3 tangent = normalize(cross(normal, vec3(0.0, 1.0, 0.0)));
   float anisoSpec = pow(abs(dot(halfDir, tangent)), 20.0) * 0.4;
   
   //  Combine lighting
   vec3 finalColor = skinColor * (0.25 + wrappedDiffuse * 0.75);  // Darker ambient
   finalColor += vec3(0.6, 0.7, 0.8) * (specular * 0.6 + anisoSpec);  // Cool highlights
   
   //  Apply vertex color modulation
   finalColor *= Kd.rgb;
   
   //  Fresnel rim lighting for ethereal glow
   float dotNV = max(dot(normal, viewDir), 0.0);
   float rim = pow(1.0 - dotNV, 4.0);
   finalColor += fissureColor * rim * glowIntensity * 0.6;
   
   //  Inner glow based on depth (energy within the creature)
   float depthGlow = smoothstep(0.0, 0.8, VertexDepth) * 0.15;
   finalColor += fissureColor * depthGlow * glowIntensity;
   
   //  Pulsing core energy
   float corePulse = sin(time * 4.0) * 0.5 + 0.5;
   finalColor += fissureColor * corePulse * (1.0 - VertexDepth * 0.3) * 0.15 * glowIntensity;
   
   //  Darker overall tone for biomechanical look
   finalColor *= 0.9;
   
   //  Output
   gl_FragColor = vec4(finalColor, Kd.a);
}
