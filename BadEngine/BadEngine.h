#pragma once

#include <string>
#include "Camera.h"
#include "const.h"
#include "FileCopier.h"
#include "OBJParser.h"
#include "Shader.h"
#include "Sphere.h"
#include <functional>


struct SimpleBox
{
  SimpleBox() : pos(0.f) {}
  glm::vec3 pos;
};

class BadEngine
{
public:
  BadEngine(std::function<void(int, int, int, int)> logic_key_handler_cb);
  ~BadEngine();

public:
  void init();
  void run();
  void draw();

public:
  bool loop_done() const;
  operator bool() const;

public:
  int get_status() const { return status_; }
  std::string get_message() const { return msg_; }

public:
  Sphere *get_sphere(size_t id) const;
  void set_sphere_radius(float rad) { sphere_rad_ = rad; }
  float get_sphere_radius(float rad) const { return sphere_rad_; }
  void set_sphere_pos(int id, float x, float y, float z);
  void set_sphere_velocity(int id, float x, float y, float z);
  void set_sphere_acc(int id, float x, float y, float z);
  size_t add_sphere(float x, float y, float z);
  void set_world_dims(glm::vec3 dims);

private:
  void demo_add_spheres();
  void process_input();
  void init_sphere_program();
  void draw_sphere_program(const glm::mat4 &view_trans, const glm::mat4 &projection_trans);
  void init_boxes_program();
  void draw_boxes_program(const glm::mat4 &view_trans, const glm::mat4 &projection_trans);
  void init_cube_program();
  void draw_cube_program(const glm::mat4 &view_trans, const glm::mat4 &projection_trans);

private:
  static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

private:
  GLFWwindow *window_;
  int status_;
  std::string msg_;
  Camera *cam_;

  OBJParser parser_;
  std::vector<Sphere *> spheres_;
  Shader sphere_shader_programme_;
  GLuint sphere_vao_ = 0;
  size_t sphere_count_ = 0;
  float sphere_rad_;

  Shader box_shader_programme_;
  GLuint box_vao_ = 0;
  size_t box_count_ = 0;

  Shader cube_shader_programme_;
  GLuint cube_vbos_[2];
  GLuint cube_vao_[1];
  glm::vec3 cube_scale_;
  SimpleBox cube_;
  std::function<void(int, int, int, int)> logic_key_handler_cb_;
  const GLuint screen_width_ = 1920;
  const GLuint screen_height_ = 1080;
  FileCopier file_copier_;
};
