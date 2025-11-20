// Lunaris Vex Vertex Shader
// Bioluminescent alien skin with subsurface scattering simulation
#version 120

//  Animation uniforms
uniform float time;
uniform float glowIntensity;
uniform int emotion;
uniform vec3 characterPos;

//  Output to fragment shader
varying vec3 View;
varying vec3 Norm;
varying vec3 WorldPos;
varying vec4 Kd;
varying vec2 TexCoord;
varying float VertexDepth;

void main()
{
   //  Breathing/pulsing effect on skin
   vec3 displacedPos = gl_Vertex.xyz;
   float breathe = sin(time * 1.5 + length(gl_Vertex.xyz) * 2.0) * 0.008;
   displacedPos += gl_Normal * breathe * glowIntensity;
   
   //  Subtle bioluminescent ripple across surface
   float ripple = sin(time * 3.0 + gl_Vertex.y * 10.0 + gl_Vertex.x * 5.0) * 0.004;
   displacedPos += gl_Normal * ripple * glowIntensity;
   
   //  Vertex location in modelview coordinates
   vec4 P = gl_ModelViewMatrix * vec4(displacedPos, 1.0);
   
   //  Normal vector
   Norm = normalize(gl_NormalMatrix * gl_Normal);
   
   //  Eye position (view direction)
   View = -P.xyz;
   
   //  World position
   WorldPos = P.xyz;
   
   //  Vertex depth for SSS
   VertexDepth = length(gl_Vertex.xyz);
   
   //  Set diffuse to Color
   Kd = gl_Color;
   
   //  Texture coordinates
   TexCoord = gl_MultiTexCoord0.xy;
   
   //  Set transformed vertex location
   gl_Position = gl_ProjectionMatrix * P;
}
