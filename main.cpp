#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

struct mesh {
  GLuint VAO;
  GLuint vertex_buffer;
  GLuint shader_program;
};

void processInput(GLFWwindow *window);
GLuint getShaderProgram(const char *vertexFile, const char *fragmentFile);
GLuint make_shader(GLenum type, const char *filename);
GLuint make_program(GLuint vertex_shader, GLuint fragment_shader);
void show_info_log(GLuint object);
void *file_contents(const char *filename, GLint *length);
mesh make_mesh(float *vertices, int size, const char *vertexFile, const char *fragmentFile);

int main()
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

  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress)))
    {
      return -1;
    }
  glViewport(0, 0, 800, 600);

  float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f, 1.0f, 0.0f
  };

  mesh triangle = make_mesh(vertices, sizeof(vertices)/sizeof(float), "main.v.glsl", "main.f.glsl");
  
  glBindVertexArray(triangle.VAO);
  glUseProgram(triangle.shader_program);
  
  while(!glfwWindowShouldClose(window))
    {
      processInput(window);

      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glDrawArrays(GL_TRIANGLES, 0, 3);
    
      glfwSwapBuffers(window);
      glfwPollEvents();
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  unsigned int shaderProgram;
  shaderProgram = getShaderProgram("main.v.glsl", "main.f.glsl");

  glBindVertexArray(0);

  mesh m;
  m.VAO = VAO;
  m.shader_program = shaderProgram;
  return m;
}
