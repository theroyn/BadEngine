#pragma once


#include "gl_incs.h"
#include <vector>
#include <string>

using namespace std;
class Shader
{
public:
  Shader() {}
  Shader(const char *vertex_path, const char *fragment_path);
  void load_program(GLint &result, std::vector<GLchar> &msg, const char *vertex_path, const char *fragment_path);
  bool get_error() const { return error_; }
  bool get_status() const { return result_; }
  vector<GLchar> get_message() const { return msg_; }
  GLint exit_error();

  void use() const { glUseProgram(program_); }

  void set_bool(const std::string &name, bool value) const;
  void set_int(const std::string &name, int value) const;
  void set_float(const std::string &name, float value) const;
  void set_vec2(const std::string &name, const glm::vec2 &value) const;
  void set_vec2(const std::string &name, float x, float y) const;
  void set_vec3(const std::string &name, const glm::vec3 &value) const;
  void set_vec3(const std::string &name, float x, float y, float z) const;
  void set_vec4(const std::string &name, const glm::vec4 &value) const;
  void set_vec4(const std::string &name, float x, float y, float z, float w);
  void set_mat2(const std::string &name, const glm::mat2 &mat) const;
  void set_mat3(const std::string &name, const glm::mat3 &mat) const;
  void set_mat4(const std::string &name, const glm::mat4 &mat) const;
  void set_mat4(const std::string &name, const void *mat) const;

private:
  GLuint load_program(GLuint vertShader, GLuint fragShader, GLint &result, vector<GLchar> &msg);
  GLuint load_shader(const char *path, GLenum type, GLint &result, vector<GLchar> &msg);

private:
  GLuint program_ = 0, vert_shader_ = 0, frag_shader_ = 0;
  GLint result_ = GL_TRUE;
  vector<GLchar> msg_;
  bool error_ = false;
};
