#include "Camera.h"
#include <iostream>

using namespace std;

Camera::Camera(GLFWwindow *window,
               const glm::vec3 &pos,
               const glm::vec3 &front,
               const glm::vec3 &world_up) :
  window_(window),
  pos_(std::move(pos)),
  orig_front_(std::move(front)),
  world_up_(std::move(world_up)),
  pitch_(0.),
  yaw_(0.),
  cursor_(0., 0.),
  fov_(DEFAULT_ZOOM),
  zoom_speed_base_(1000.f),
  translation_speed_base_(5.f),
  spin_speed_base_(7.5f),
  is_cursor_init_(false)
{
  up_ = world_up_;
  front_ = orig_front_;

  spin_speed_ = spin_speed_base_;
  zoom_speed_ = zoom_speed_base_;
  translation_speed_ = translation_speed_base_;

  glfwGetWindowSize(window_, &screen_w_, &screen_h_);
}


void Camera::step_forward(int dir)
{
  glm::vec3 forward = glm::normalize(front_);

  glm::vec3 inc = forward * translation_speed_ * ( float )dir;
  inc.y = 0;

  pos_ += inc;
  update_view();
}

void Camera::step_right(int dir)
{
  glm::vec3 forward = glm::normalize(front_);
  glm::vec3 right = glm::normalize(glm::cross(forward, world_up_));

  /**
   * right.y is always 0 because world_up_.xz are always(0,0).
   * If world_up_ changes an update will be neccessary.
   */
  pos_ += right * translation_speed_ * ( float )dir;
  update_view();
}

void Camera::increase_pos(const glm::vec3 &pos)
{
  pos_ += translation_speed_ * pos;
}

void Camera::process_keyboard_input()
{
  // Move forward/backward.
  if (int key_u = glfwGetKey(window_, GLFW_KEY_W),
      key_d = glfwGetKey(window_, GLFW_KEY_S);
      GLFW_PRESS == key_u || GLFW_PRESS == key_d)
  {
    int dir = (key_u == GLFW_PRESS) ? 1 : -1;
    step_forward(dir);
  }

  // Move right/left.
  if (int key_r = glfwGetKey(window_, GLFW_KEY_D),
      key_l = glfwGetKey(window_, GLFW_KEY_A);
      GLFW_PRESS == key_r || GLFW_PRESS == key_l)
  {
    int dir = (key_r == GLFW_PRESS) ? 1 : -1;
    step_right(dir);
  }

  // Reset zoom on MMB.
  if (int state = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE);
      state == GLFW_PRESS)
    reset_zoom();
}

void Camera::zoom(float y_off)
{
  fov_ -= zoom_speed_ * y_off;

  if (float upper_lim = 180.f; fov_ + CAMERA_EPSILON > upper_lim)
    fov_ = upper_lim - CAMERA_EPSILON;
  else if (fov_ - CAMERA_EPSILON < 0)
    fov_ = CAMERA_EPSILON;
}

void Camera::reset_zoom()
{
  fov_ = DEFAULT_ZOOM;
}

float Camera::get_fov() const
{
  float fov = glm::radians(fov_);

  return fov;
}

void Camera::update_view()
{
  glm::mat4 m_pitch = glm::rotate(glm::mat4(), glm::radians(( float )pitch_), glm::vec3(1., 0., 0.));
  glm::mat4 m_yaw = glm::rotate(glm::mat4(), glm::radians(( float )yaw_)  , glm::vec3(0., 1., 0.));

  front_ = glm::normalize(m_yaw * m_pitch * glm::vec4(orig_front_, 1.f));
  glm::vec3 right = glm::normalize(glm::cross(front_, world_up_));
  up_ = glm::normalize(glm::cross(right, front_));
  /*cout << "front_:" << front_.x << ", " << front_.y << ", " << front_.z << endl;
  cout << "right:" << right.x << ", " << right.y << ", " << right.z << endl;
  cout << "up_:" << up_.x << ", " << up_.y << ", " << up_.z << endl;*/
}

glm::mat4 Camera::get_view() const
{
  //glm::mat4 view = glm::lookAt(pos_, pos_ + front_, up_);
  glm::mat4 view = get_lookat();

  return view;
}

glm::mat4 Camera::get_projection() const
{
  GLfloat p = (GLfloat)glfwGetTime();
  float x = 0.f, z = 0.f;
  static float radius = 10.f;

  x = sin(p)*radius;
  z = cos(p)*radius;

  glm::mat4 projection = glm::perspective(get_fov(),
                                          (float)screen_w_ / (float)screen_h_,
                                          .1f, 100.f);

  return projection;
}

glm::vec3 Camera::get_pos() const
{

  return pos_;
}

glm::mat4 Camera::get_lookat() const
{
  glm::mat4 lookat(1.);

  glm::vec3 f = glm::normalize(front_);

  glm::vec3 r = glm::normalize(glm::cross(f, up_));
  glm::vec3 u = glm::cross(r, f);
  const glm::vec3 &p = pos_;

  lookat[0][0] = r.x;
  lookat[1][0] = r.y;
  lookat[2][0] = r.z;
  lookat[0][1] = u.x;
  lookat[1][1] = u.y;
  lookat[2][1] = u.z;
  lookat[0][2] = -f.x;
  lookat[1][2] = -f.y;
  lookat[2][2] = -f.z;
  lookat[3][0] = -glm::dot(r, p);
  lookat[3][1] = -glm::dot(u, p);
  lookat[3][2] = glm::dot(f, p);

  return lookat;
}

void Camera::set_cursor(double x, double y)
{

  if (!is_cursor_init_)
  {
    cursor_.x = (float)x;
    cursor_.y = (float)y;
    is_cursor_init_ = true;
  }

  float xoff = (float)x - cursor_.x;
  float yoff = (float)y - cursor_.y;

  cursor_.x = (float)x;
  cursor_.y = (float)y;
  
  //increase_pitch(yoff);
  increase_yaw(xoff);
  offset_angles(xoff, yoff);
}

void Camera::offset_angles(float xoff, float yoff)
{
  pitch_ -= spin_speed_ * yoff;
  yaw_ -= spin_speed_ * xoff;

  if (pitch_ > 89.)
    pitch_ = 89.;
  else if (pitch_ < -89.)
    pitch_ = -89.;
}

void Camera::increase_pitch(float off)
{
  //cout << "pitch:" << pitch_ << ", off:" << spin_speed_ * off << endl;
  pitch_ -= spin_speed_ * off;

  if (pitch_ > 89.)
    pitch_ = 89.;
  else if (pitch_ < -89.)
    pitch_ = -89.;

  update_view();
}
void Camera::increase_yaw(float off)
{
  //cout << "yaw:" << yaw_ << ", radians(yaw):" << glm::radians(yaw_) << ", off:" << spin_speed_ * off << endl;
  //yaw_ = glm::mod(yaw_ + off, 360.);
  yaw_ -= spin_speed_ * off;

  update_view();
}


void Camera::update_time_deltas()
{
  GLfloat curr_time = (GLfloat)glfwGetTime();
  delta_time_ = curr_time - last_frame_;
  last_frame_ = curr_time;

  spin_speed_ = spin_speed_base_ * delta_time_;
  translation_speed_ = translation_speed_base_ * delta_time_;
  zoom_speed_ = zoom_speed_base_ * delta_time_;
}