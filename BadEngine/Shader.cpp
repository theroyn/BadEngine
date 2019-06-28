#include "Shader.h"
#include "utils.h"
#include "const.h"

GLint Shader::exit_error()
{
  GLint status = error_ ? R_FAILURE : R_SUCCESS;

  if (error_)
    utility::dbg_print(&((msg_)[0]));

  return status;
}

Shader::Shader(const char *vertex_path, const char *fragment_path) :
  program_(0),
  result_(GL_TRUE)
{
  load_program(result_, msg_, vertex_path, fragment_path);
}

void Shader::load_program(GLint &result, vector<GLchar>  &msg,
                          const char *vertex_path, const char *fragment_path)
{
  program_ = 0;

  vert_shader_ = load_shader(vertex_path, GL_VERTEX_SHADER, result, msg);


  if (result == GL_TRUE)
    frag_shader_ = load_shader(fragment_path, GL_FRAGMENT_SHADER, result, msg);
  if (result == GL_TRUE)
    program_ = load_program(vert_shader_, frag_shader_, result, msg);

  error_ = (result == GL_FALSE);
}

GLuint Shader::load_program(GLuint vertShader, GLuint fragShader, GLint &result, std::vector<GLchar> &msg)
{
  GLuint program = glCreateProgram();

  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &result);

  if (result == GL_FALSE)
  {
    int log_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    msg.resize(log_length);
    glGetProgramInfoLog(program, log_length, NULL, &msg[0]);
  }

  return program;
}


GLuint Shader::load_shader(const char *path, GLenum type, GLint &result, std::vector<GLchar> &msg)
{
  GLuint shader = glCreateShader(type);
  string shaderStr = utility::read_file(path);
  const char *shaderSrc = shaderStr.c_str();

  int log_length = 0;;

  // Compile vertex shader
  utility::dbg_print("Compiling shader.");
  glShaderSource(shader, 1, &shaderSrc, NULL);
  glCompileShader(shader);

  // Check shader
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE)
  {
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    msg.resize(log_length);
    glGetShaderInfoLog(shader, log_length, NULL, &msg[0]);
  }
  else
  {
    utility::dbg_print("Successful compilation.");
  }

  return shader;
}

void Shader::set_bool(const std::string &name, bool value) const
{
  glUniform1i(glGetUniformLocation(program_, name.c_str()), ( int )value); 
}

void Shader::set_int(const std::string &name, int value) const
{
  glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
}

void Shader::set_float(const std::string &name, float value) const
{
  glUniform1f(glGetUniformLocation(program_, name.c_str()), value);
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const
{
  glUniform2fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
}
void Shader::set_vec2(const std::string &name, float x, float y) const
{
  glUniform2f(glGetUniformLocation(program_, name.c_str()), x, y);
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const
{
  glUniform3fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
}
void Shader::set_vec3(const std::string &name, float x, float y, float z) const
{
  glUniform3f(glGetUniformLocation(program_, name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const
{
  glUniform4fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
}
void Shader::set_vec4(const std::string &name, float x, float y, float z, float w)
{
  glUniform4f(glGetUniformLocation(program_, name.c_str()), x, y, z, w);
}

void Shader::set_mat2(const std::string &name, const glm::mat2 &mat) const
{
  glUniformMatrix2fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string &name, const glm::mat3 &mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const
{
  auto loc = glGetUniformLocation(program_, name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat) /** &mat[0][0] */);
}

//void Shader::set_tex(const std::string &name, const string &path, bool alpha) const
//{
//  GLuint texture = utility::load_texture(path.c_str(), result_, msg_, alpha);
//  error_ = !result_;
//}