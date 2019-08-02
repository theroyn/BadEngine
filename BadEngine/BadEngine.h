#pragma once

#include <string>
#include "Camera.h"
#include "const.h"
#include "OBJParser.h"
#include "Shader.h"
#include "Sphere.h"


struct SimpleBox
{
  SimpleBox() : pos(0.f) {}
  glm::vec3 pos;
};

class BadEngine
{
public:
  BadEngine();
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
  Sphere *get_sphere(int id) const;
  void    set_sphere_radius(float rad) { sphere_rad_ = rad; }
  float   get_sphere_radius(float rad) const { return sphere_rad_; }
  void    set_sphere_pos(int id, float x, float y, float z);
  void    set_sphere_velocity(int id, float x, float y, float z);
  void    set_sphere_acc(int id, float x, float y, float z);
  int     add_sphere(float x, float y, float z);
  void    set_world_dims(glm::vec3 dims);

private:
  void demo_add_spheres();
  void process_input();

private:
  GLFWwindow *window_;
  int status_;
  std::string msg_;
  Camera *cam_;
  OBJParser parser_;
  std::vector<Sphere *> spheres_;
  Shader sphere_shader_programme_;
  GLuint sphere_vbos_[2];
  GLuint sphere_vao_[1];
  std::vector<unsigned int> sphere_indices_;
  std::vector<float> sphere_data_;
  float sphere_rad_;

  Shader box_shader_programme_;
  GLuint box_vbos_[2];
  GLuint box_vao_[1];
  std::vector<unsigned int> box_indices_;
  std::vector<float> box_data_;
  glm::vec3 box_scale_;
  SimpleBox box_;
};
