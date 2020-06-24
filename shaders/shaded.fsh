#version 460 core
#define SOLID_COLOR 1
#define NO_SHADE 4

in vec3 col;
in vec2 uv;
in vec3 normal_cs;
//in vec3 light_dir_cs;
in vec3 pos;


uniform sampler2D tex_sampler;
uniform vec3 lightPos[4];  
uniform int flags;
uniform vec3 u_col;
uniform vec3 viewPos;
uniform vec3 light_pos;

out vec4 fragColor;

float light_power = 3;
 
void main() {


  vec3 lightColor = vec3(1.0,1.0,0.9);
  float constant =1.0f;
  float linear= 	0.14;
  float quadratic =0.07;
  float specularStrength = 0.2;
  vec3 norm  = normalize(normal_cs);
  vec3 viewDir = normalize(viewPos - pos);
  float ambientStrength = 0.1;
  vec3 multiplier=vec3(0);


  vec4 mcol = texture(tex_sampler, uv).rgba;


  for(int i=0;i<4;i++){
      vec3 ambient = ambientStrength * lightColor;

      float distance = length(lightPos[i] - pos);
      float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));  


      vec3 lightDir = normalize(lightPos[i] - pos); 
      vec3 reflectDir = reflect(-lightDir, norm);  



      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
      vec3 specular = specularStrength * spec * lightColor;  

      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = diff * lightColor;


      ambient  *= attenuation; 
      diffuse  *= attenuation;
      specular *= attenuation; 
      multiplier+= (ambient+diffuse+specular);

      multiplier+= (ambient+specular+diffuse);
  }

  fragColor = vec4(mcol.rgb*multiplier,mcol.a);

  }