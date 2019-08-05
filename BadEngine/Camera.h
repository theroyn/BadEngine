#ifndef R_CAMERA_H
#define R_CAMERA_H

#include "gl_incs.h"

#define CAMERA_EPSILON 0.0000001f
#define DEFAULT_ZOOM 45.

class Camera
{
public:
  Camera(GLFWwindow *window,
         const glm::vec3 &pos,
         const glm::vec3 &front,
         const glm::vec3 &world_up);

public:
  glm::mat4 get_view() const;
  glm::mat4 get_projection() const;
  glm::vec3 get_pos() const;
  void increase_pos(const glm::vec3 &pos);
  void update_view();
  void set_cursor(double x, double y);
  void process_keyboard_input();
  void reset_zoom();
  void zoom(float y_off);
  void update_time_deltas();

private:
  float get_fov() const;
  void increase_pitch(float pitch);
  void increase_yaw(float yaw);
  void offset_angles(float xoff, float yoff);
  void step_forward(int dir);
  void step_right(int dir);
  glm::mat4 get_lookat() const;

private:
  float pitch_, yaw_;
  float fov_;
  float spin_speed_, translation_speed_, zoom_speed_;
  const float zoom_speed_base_, translation_speed_base_, spin_speed_base_;
  float last_frame_ = 0.f, delta_time_ = 0.f;
  bool is_cursor_init_;

  int screen_w_, screen_h_;

private:
  glm::vec3 world_up_;
  glm::vec3 pos_;
  glm::vec3 front_;
  glm::vec3 orig_front_;
  glm::vec3 up_;
  glm::vec2 cursor_;

  GLFWwindow *window_;

};

#endif