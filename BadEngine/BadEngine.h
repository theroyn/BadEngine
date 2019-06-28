#pragma once

#include <string>
#include "Camera.h"
#include "const.h"
#include "OBJParser.h"
#include "Shader.h"

struct Sphere
{
  Sphere(float x, float y, float z) : pos(x, y, z), vel(0.f) {}

  glm::vec3 pos;
  glm::vec3 vel;
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
  void    set_sphere_pos(int id, float x, float y, float z);
  void    set_sphere_velocity(int id, float x, float y, float z);
  int     add_sphere(float x, float y, float z);

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
};
