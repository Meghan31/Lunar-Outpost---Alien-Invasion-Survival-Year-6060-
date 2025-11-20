// Alien Scout Drone Vertex Shader
// Handles vertex transformation with energy distortion
#version 120

//  Animation uniforms
uniform float time;
uniform vec3 dronePos;
uniform float energyLevel;

//  Output to fragment shader
varying vec3 View;
varying vec3 Norm;
varying vec3 WorldPos;
varying vec4 Kd;
varying vec2 TexCoord;
varying float DistFromCenter;

void main()
{
   //  Calculate distance from drone center for energy effects
   DistFromCenter = length(gl_Vertex.xyz);
   
   //  Apply energy field distortion (pulsing)
   vec3 displacedPos = gl_Vertex.xyz;
   float pulse = sin(time * 4.0 + DistFromCenter * 3.0) * 0.015 * energyLevel;
   displacedPos += gl_Normal * pulse;
   
   //  Electromagnetic interference ripple
   float ripple = sin(time * 8.0 + gl_Vertex.y * 10.0) * 0.008;
   displacedPos.x += ripple * energyLevel;
   
   //  Vertex location in modelview coordinates
   vec4 P = gl_ModelViewMatrix * vec4(displacedPos, 1.0);
   
   //  Normal vector
   Norm = normalize(gl_NormalMatrix * gl_Normal);
   
   //  Eye position (view direction)
   View = -P.xyz;
   
   //  World position
   WorldPos = P.xyz;
   
   //  Set diffuse to Color
   Kd = gl_Color;
   
   //  Texture coordinates
   TexCoord = gl_MultiTexCoord0.xy;
   
   //  Set transformed vertex location
   gl_Position = gl_ProjectionMatrix * P;
}
