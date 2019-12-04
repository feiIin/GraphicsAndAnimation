#version 430 core

out vec4 color;

in VS_OUT
{
    vec2 tc;
    vec3 normals;
	vec3 pos;
} fs_in;

uniform mat4 model_matriLightsx;
layout(binding=0) uniform sampler2D tex;


//Lights---------------------------------------------------

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

//Lights---------------------------------------------------

void main(void){

  // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
 
    // diffuse
    vec3 norm = normalize(fs_in.normals);
    vec3 lightDir = normalize(lightPos - fs_in.pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
   
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fs_in.pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
       
    vec3 result = (ambient + diffuse + specular);// * objectColor;
    color = vec4(result, 1.0) * texture(tex, fs_in.tc);

}
