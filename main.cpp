#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "stb_image.h"

struct mesh {
  GLuint VAO;
  GLuint vertex_buffer;
  GLuint shader_program;
};

struct canvas {
  unsigned char *data;
  int width, height;
};

struct vector2 {
  float x, y;
};

struct projectiles {
  vector2 pos;
  vector2 v;
  vector2 g;
  char r, gr, b;
};


void processInput(GLFWwindow *window);
GLuint getShaderProgram(const char *vertexFile, const char *fragmentFile);
GLuint make_shader(GLenum type, const char *filename);
GLuint make_program(GLuint vertex_shader, GLuint fragment_shader);
void show_info_log(GLuint object);
void *file_contents(const char *filename, GLint *length);
mesh make_mesh(float *vertices, int size, const char *vertexFile, const char *fragmentFile);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);  

void putpixel(canvas c, int x, int y, char r, char g, char b);

canvas newcanvas(int w, int h);
void generateStatic(canvas screen);
void updateCanvas(canvas screen);

vector2 operator+(vector2 a, vector2 b);
vector2 operator*(float a, vector2 b);


int main(int argc, char **argv)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
    {
      glfwTerminate();
      return -1;
    }
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress)))
    {
      return -1;
    }
  
  glViewport(0, 0, 800, 600);
  
  float vertices[] = {
                      -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                      1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                      -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
                      -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                      1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                      1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f                    
  };
  
  mesh triangle = make_mesh(vertices, sizeof(vertices)/sizeof(float), argv[1], "main.f.glsl");  
  glBindVertexArray(triangle.VAO);
  glUseProgram(triangle.shader_program);

  GLuint timer = glGetUniformLocation(triangle.shader_program, "timer");
  
  unsigned int texture;  
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  canvas screen = newcanvas(300, 300);     
  float startTime = glfwGetTime();
  float elapsedTime = 0;
  float totalElapsed = 0;
  float ems = 0;
  while(!glfwWindowShouldClose(window))
    {
      startTime = glfwGetTime();
      
      processInput(window);      
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      generateStatic(screen);
      //updateCanvas(screen);

      glDrawArrays(GL_TRIANGLES, 0, 6);
    
      glfwSwapBuffers(window);      
      glfwPollEvents();     

      elapsedTime = glfwGetTime() - startTime;

      ems = elapsedTime;
      std::cout<<ems<<std::endl;
      
      totalElapsed += elapsedTime;      
      glUniform1f(timer, totalElapsed);      
    }

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

GLuint getShaderProgram(const char *vertexFile, const char *fragmentFile)
{
  GLuint vertexShader = make_shader(GL_VERTEX_SHADER, vertexFile);
  GLuint fragmentShader = make_shader(GL_FRAGMENT_SHADER, fragmentFile);
  GLuint shaderProgram = make_program(vertexShader, fragmentShader);
  
  return shaderProgram;
}

GLuint make_shader(GLenum type, const char *filename)
{
  GLint length;
  GLchar *source = (GLchar *)file_contents(filename, &length);
  GLuint shader;
  GLint shader_ok;

  if(!source) return 0;
  shader = glCreateShader(type);
  glShaderSource(shader, 1, (const GLchar**)&source, &length);
  free(source);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
  if(!shader_ok) {
    fprintf(stderr, "Failed to compile %s: \n", filename);
    show_info_log(shader);
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

void show_info_log(GLuint object)
{
  GLint log_length;
  char *log;

  glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  log = (char*)malloc(log_length);
  glGetShaderInfoLog(object, log_length, NULL, log);
  fprintf(stderr, "%s", log);
  free(log); 
}

GLuint make_program(GLuint vertex_shader, GLuint fragment_shader)
{
  GLint program_ok;
  GLuint program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  return program;
}

void *file_contents(const char *filename, GLint *length) {
  FILE *f = fopen(filename, "r");
  void *buffer;

  if(!f) {
    fprintf(stderr, "Unable to open %s ofr reading\n", filename);
    return NULL;    
  }

  fseek(f, 0, SEEK_END);
  *length = ftell(f);
  fseek(f, 0, SEEK_SET);

  buffer = malloc(*length + 1);
  *length = fread(buffer, 1, *length, f);
  fclose(f);
  ((char *)buffer)[*length]= '\0';

  return buffer;
}

mesh make_mesh(float *vertices, int size, const char *vertexFile, const char *fragmentFile)
{
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, size*sizeof(float), vertices, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3*sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

  unsigned int shaderProgram;
  shaderProgram = getShaderProgram("main.v.glsl", "main.f.glsl");

  glBindVertexArray(0);

  mesh m;
  m.VAO = VAO;
  m.shader_program = shaderProgram;
  return m;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

canvas newcanvas(int w, int h) {
  canvas c;
  c.width = w;
  c.height = h;
  c.data = (unsigned char *)malloc(w*h*3);
  return c;
}

void putpixel(canvas c, int x, int y, char r, char g, char b) {
  if(x >= 0 && y >= 0 && x < c.width && y < c.height) {
    int loc = (y*c.width+x)*3;
    c.data[loc] = r;
    c.data[loc + 1] = g;
    c.data[loc + 2] = b;
  }
}

void generateStatic(canvas screen) {
  for(int y=0;y<screen.height;y++)
    for(int x=0;x<screen.width;x++)
      {
        char r = rand()%255+1;
        char g = rand()%255+1;
        char b = rand()%255+1;

        putpixel(screen, x, y, r, r, r);
      }
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.width, screen.height, 0, GL_RGB, GL_UNSIGNED_BYTE, screen.data);  
  glGenerateMipmap(GL_TEXTURE_2D);
}

void updateCanvas(canvas screen) {
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.width, screen.height, 0, GL_RGB, GL_UNSIGNED_BYTE, screen.data);  
  glGenerateMipmap(GL_TEXTURE_2D);
}

vector2 operator+(vector2 a, vector2 b) {
  vector2 c;
  c.x = a.x + b.x;
  c.y = a.y + b.y;
  return c;
}

vector2 operator*(float a, vector2 b) {
  vector2 c;
  c.x = a*b.x;
  c.y = a*b.y;
  return c;
}
