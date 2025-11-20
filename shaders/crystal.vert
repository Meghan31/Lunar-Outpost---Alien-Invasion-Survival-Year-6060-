// Crystal Vertex Shader
// Handles vertex transformation and passes data to fragment shader
#version 120

//  Animation uniforms
uniform float time;
uniform vec3 cameraPos;

//  Output to fragment shader
varying vec3 View;
varying vec3 Norm;
varying vec3 WorldPos;
varying vec4 Kd;
varying vec2 TexCoord;

void main()
{
   //  Apply subtle vertex displacement for pulsing effect
   vec3 displacedPos = gl_Vertex.xyz;
   float pulse = sin(time * 3.0 + gl_Vertex.y * 2.0) * 0.02;
   displacedPos += gl_Normal * pulse;
   
   //  Vertex location in modelview coordinates
   vec4 P = gl_ModelViewMatrix * vec4(displacedPos, 1.0);
   
   //  Normal vector
   Norm = normalize(gl_NormalMatrix * gl_Normal);
   
   //  Eye position (view direction)
   View = -P.xyz;
   
   //  World position (approximated from modelview)
   WorldPos = P.xyz;
   
   //  Set diffuse to Color
   Kd = gl_Color;
   
   //  Texture coordinates
   TexCoord = gl_MultiTexCoord0.xy;
   
   //  Set transformed vertex location
   gl_Position = gl_ProjectionMatrix * P;
}
