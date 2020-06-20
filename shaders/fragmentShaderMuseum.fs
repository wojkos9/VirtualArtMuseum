#version 460 core

out vec4 FragColor;


in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos[4];  
uniform sampler2D textureOfWall;
uniform vec3 viewPos;



void main()
{
   vec3 lightColor = vec3(1.0,1.0,0.9);
   float constant =1.0f;
   float linear= 	0.14;
   float quadratic =0.07;
   float specularStrength = 0.5;
   vec3 norm = normalize(Normal);
   vec3 viewDir = normalize(viewPos - FragPos);
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * lightColor;

   vec3 multiplier=vec3(0);
  
   for(int i=0;i<4;i++){
      float distance    = length(lightPos[i] - FragPos);
      float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));  

      vec3 lightDir = normalize(lightPos[i] - FragPos); 
      vec3 reflectDir = reflect(-lightDir, norm);  

      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
      vec3 specular = specularStrength * spec * lightColor;  

      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = diff * lightColor;
      
      ambient  *= attenuation; 
      diffuse  *= attenuation;
      specular *= attenuation; 
      multiplier+= (ambient+diffuse+specular);
   }
  
   vec3 result = vec3(texture(textureOfWall, TexCoord))*(multiplier);
   FragColor = vec4(result,1.0f);
 //  FragColor = (texture(textureOfWall, TexCoord));
}