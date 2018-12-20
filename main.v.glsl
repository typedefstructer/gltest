#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform float timer;
out vec4 vertexColor;
out vec2 texCoord;

void main()
{
  mat3 window_scale = mat3(
    vec3(3.0/4.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
  );

  mat3 object_scale = mat3(
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
  );

  mat3 rotation = mat3(
    vec3(cos(timer), sin(timer), 0.0),
    vec3(-sin(timer), cos(timer), 0.0),
    vec3(0.0, 0.0, 1.0)
  );
    
  gl_Position = vec4(window_scale*aPos.xyz, 1.0);
  vertexColor = vec4(aColor.xyz , 1.0);
  texCoord = aTexCoord;
}