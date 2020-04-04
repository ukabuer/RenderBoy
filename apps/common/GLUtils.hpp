#pragma once
#include <glad/glad.h>
#include <iostream>
#include <stdexcept>
#include <string>

inline void PrintGLErrorType(GLenum glerr) {
  std::string error;
  switch (glerr) {
  case GL_INVALID_ENUM:
    error = "INVALID_ENUM";
    break;
  case GL_INVALID_VALUE:
    error = "INVALID_VALUE";
    break;
  case GL_INVALID_OPERATION:
    error = "INVALID_OPERATION";
    break;
  case GL_STACK_OVERFLOW:
    error = "STACK_OVERFLOW";
    break;
  case GL_STACK_UNDERFLOW:
    error = "STACK_UNDERFLOW";
    break;
  case GL_OUT_OF_MEMORY:
    error = "OUT_OF_MEMORY";
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    error = "INVALID_FRAMEBUFFER_OPERATION";
    break;
  default:
    error = "UNKNOWN_ERROR";
    break;
  }
  std::cout << error << std::endl;
}

#ifdef NDEBUG
#define GL_REPORT void(0);
#define GL_ASSERT void(0);
#define GL_EXPR(stmt) stmt;
#define GL_CHECK void(0);
#else
#define GL_ASSERT assert(glGetError() == GL_NO_ERROR);

#define GL_REPORT PrintGLErrorMsg(__FILE__, __LINE__);
//{												\
	//	GLenum err;									\
	//	while((err = glGetError()) != GL_NO_ERROR)	\
	//	{											\
	//		ysl::Warning("OpenGL Error Code:{}. File:{}, Line:{}. \n",err,__FILE__,__LINE__);\
	//	}											\
	//}

#define GL_ASSERT assert(glGetError() == GL_NO_ERROR);

#define GL_EXPR(stmt)                                                          \
  do {                                                                         \
    GLenum glerr;                                                              \
    unsigned int iCounter = 0;                                                 \
    while ((glerr = glGetError()) != GL_NO_ERROR) {                            \
      std::cout << "GL error calling " << #stmt << " before line " << __LINE__ \
                << " of " << __FILE__ << ": (" << static_cast<unsigned>(glerr) \
                << ")\n";                                                      \
      PrintGLErrorType(glerr);                                                 \
      iCounter++;                                                              \
      if (iCounter > 100)                                                      \
        break;                                                                 \
    }                                                                          \
    stmt;                                                                      \
    iCounter = 0;                                                              \
    while ((glerr = glGetError()) != GL_NO_ERROR) {                            \
      std::cout << #stmt << " on line " << __LINE__ << " of " << __FILE__      \
                << " caused GL error: (" << static_cast<unsigned>(glerr)       \
                << ")\n";                                                      \
      PrintGLErrorType(glerr);                                                 \
      iCounter++;                                                              \
      if (iCounter > 100)                                                      \
        break;                                                                 \
    }                                                                          \
  } while (0)

#define GL_CHECK                                                               \
  do {                                                                         \
    GLenum glerr;                                                              \
    unsigned int iCounter = 0;                                                 \
    while ((glerr = glGetError()) != GL_NO_ERROR) {                            \
      std::cout << "before line (" << __LINE__ << ") in file " << __FILE__     \
                << " caused GL error: (" << static_cast<unsigned>(glerr)       \
                << ")\n";                                                      \
      PrintGLErrorType(glerr);                                                 \
      iCounter++;                                                              \
      if (iCounter > 100)                                                      \
        break;                                                                 \
    }                                                                          \
    iCounter = 0;                                                              \
    while ((glerr = glGetError()) != GL_NO_ERROR) {                            \
      std::cout << "on line (" << __LINE__ << ") in file " << __FILE__         \
                << " caused GL error: (" << static_cast<unsigned>(glerr)       \
                << ")\n";                                                      \
      PrintGLErrorType(glerr);                                                 \
      iCounter++;                                                              \
      if (iCounter > 100)                                                      \
        break;                                                                 \
    }                                                                          \
  } while (0)
#endif

inline GLuint create_shader(GLenum type, const char *shader_text) {
  GLuint shader = 0;
  GL_EXPR(shader = glCreateShader(type));
  GL_EXPR(glShaderSource(shader, 1, &shader_text, nullptr));
  glCompileShader(shader);

  int success;
  char log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, log);
    throw std::runtime_error(std::string("compile shader error: ") + log);
  };

  return shader;
}

inline GLuint create_program(GLuint vertex_shader, GLuint fragment_shader) {
  auto program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  int success;
  char log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, log);
    throw std::runtime_error(std::string("link program error: ") + log);
  }

  return program;
}

inline GLuint create_texture(int width, int height, const unsigned char *data) {
  GLuint texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  return texture;
}
