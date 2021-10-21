#version 410 core  

uniform sampler2D Texture;
uniform sampler2D CloudsTexture;

uniform float Time;
uniform vec2 CloudsRotationSpeed = vec2(0.008, 0.0);

in vec3 Color;
in vec2 UV;
in vec3 Normal;

uniform vec3 LightDirection;
uniform float LightIntensity;

out vec4 OutColor; 

void main(){

    vec3 N = normalize(Normal);
    vec3 L = -normalize(LightDirection);

    float Lambertian = max(dot(N,L),0.0);
    
    vec3 V = vec3(0.0, 0.0, 1.0);
   
    vec3 R = reflect(-L, N);

    float Alpha = 10.0f;
    float Spec= pow(max(dot(R,-V),0.0),Alpha);
    Spec = max(Spec, 0.0);

    vec3 colorSpec= vec3(0.62,0.26,0.83);

    vec3 TextureEarth = texture(Texture,UV).rgb;
    vec3 CloudColor = texture(CloudsTexture,UV + Time* CloudsRotationSpeed).rgb;
    vec3 FinalColor = (TextureEarth + CloudColor) *LightIntensity *Lambertian + Spec;

    OutColor = vec4(FinalColor, 1.0);
}