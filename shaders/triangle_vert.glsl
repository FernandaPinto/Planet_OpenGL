#version 410 core  

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec3 InColor;
layout (location = 3) in vec2 InUV;

uniform mat4 MVP ;
uniform mat4 NormalMatrix;

out vec3 Normal;
out vec3 Color;
out vec2 UV;

void main(){

   vec4 Position  = MVP * vec4(InPosition, 1.0);
   Normal = vec3(NormalMatrix *vec4 (InNormal, 0.0f));
   Color = InColor;
   UV = InUV;
   gl_Position = Position;

}
