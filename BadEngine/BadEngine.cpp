
#include <iostream>
#include <sstream>
#include <functional>

#include "gl_incs.h"

#include "utils.h"
#include "BadEngine.h"
#include "coords.h"
#include "Shader.h"

namespace gl_cbs
{
  static Camera *p_camera = nullptr;
  void mouse_drag_cb(GLFWwindow * /** Ignore */, double xpos, double ypos);
  void mouse_scroll_cb(GLFWwindow * /** Ignore */, double /** Ignore */, double y_off);
}

#define VMSG_TO_STR(msg) &((msg)[0])

void framebuffer_size_cb(GLFWwindow *window, int width, int height);

void framebuffer_size_cb(GLFWwindow *window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

BadEngine::BadEngine(std::function<void(int, int, int, int)> logic_key_handler_cb) : status_(R_SUCCESS),
                                                                                     msg_(""),
                                                                                     cam_(nullptr),
                                                                                     sphere_vbos_{ 0 },
                                                                                     sphere_vao_{ 0 },
                                                                                     sphere_rad_{ .2f },
                                                                                     //sphere_rad_ { .001f, .001f, .001f},
                                                                                     cube_vbos_{ 0 },
                                                                                     cube_vao_{ 0 },
                                                                                     cube_scale_{ 1.f, 1.f, 1.f },
                                                                                     logic_key_handler_cb_(logic_key_handler_cb)

{
}


BadEngine::~BadEngine()
{
  if (cam_)
  {
    delete cam_;
    cam_ = nullptr;
    gl_cbs::p_camera = nullptr;
  }

  spheres_.erase(spheres_.begin(), spheres_.end());
}

void BadEngine::init_sphere_program()
{
  parser_.parse("SphereRad1.obj");

  sphere_data_ = parser_.get_data();
  sphere_indices_ = parser_.get_indices();

  glGenVertexArrays(1, sphere_vao_);
  glBindVertexArray(sphere_vao_[0]);

  glGenBuffers(sizeof(sphere_vbos_) / sizeof(sphere_vbos_[0]),
               sphere_vbos_);
  glBindBuffer(GL_ARRAY_BUFFER,
               sphere_vbos_[0]);
  glBufferData(GL_ARRAY_BUFFER,
               sphere_data_.size() * sizeof(sphere_data_[0]),
               &sphere_data_[0],
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_vbos_[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sphere_indices_.size() * sizeof(sphere_indices_[0]),
               &sphere_indices_[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  sphere_shader_programme_ = Shader("base_vs.glsl", "base_fs.glsl");

  if (sphere_shader_programme_.get_error())
  {
    auto msgv = sphere_shader_programme_.get_message();
    msg_ = VMSG_TO_STR(msgv);
    status_ = sphere_shader_programme_.get_error() ? R_FAILURE : R_SUCCESS;
  }
}

void BadEngine::init_cube_program()
{
  // load containing cube
  glGenVertexArrays(1, cube_vao_);
  glBindVertexArray(cube_vao_[0]);

  glGenBuffers(sizeof(cube_vbos_) / sizeof(cube_vbos_[0]),
               cube_vbos_);
  glBindBuffer(GL_ARRAY_BUFFER,
               cube_vbos_[0]);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(normalized_cube_coords),
               normalized_cube_coords,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_vbos_[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(normalized_cube_indices),
               normalized_cube_indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void *>(0));

  glEnableVertexAttribArray(0);

  cube_shader_programme_ = Shader("containing_cube_vs.glsl", "containing_cube_fs.glsl");

  if (cube_shader_programme_.get_error())
  {
    auto msgv = cube_shader_programme_.get_message();
    msg_ = VMSG_TO_STR(msgv);
    status_ = cube_shader_programme_.get_error() ? R_FAILURE : R_SUCCESS;
  }
}

void BadEngine::init()
{
  if (glfwInit() == GL_FALSE)
  {
    status_ = R_FAILURE;
    msg_ = "Cannot initiate opengl.";
    throw std::runtime_error(msg_);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window_ = glfwCreateWindow(screen_width_, screen_height_, "Hello Texture", NULL, NULL);

  if (window_ == NULL)
  {
    status_ = R_FAILURE;
    msg_ = "Cannot create opengl window.";
    throw std::runtime_error(msg_);
  }

  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_cb);

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  int res = glewInit();

  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *version = glGetString(GL_VERSION);

  std::stringstream inf;
  inf << "Renderer:" << renderer << std::endl
      << "OpenGL version supported:" << version << std::endl;
  utility::dbg_print(inf.str());

  glm::vec3 camera_pos(2.f, 2.f, 10.f);
  glm::vec3 camera_front(-.3f, -.3f, -1.f);
  glm::vec3 world_up(0.f, 1.f, 0.f);

  cam_ = new Camera(window_, camera_pos, camera_front, world_up);
  gl_cbs::p_camera = cam_;

  //parser_.parse("TriangulatedSphere.obj");
  //demo_add_spheres();
  // parser_.parse("cube.obj");

  glEnable(GL_DEPTH_TEST);
  glClearColor(.7f, .8f, .5f, 1.f);
  // fix alignment for subsequent calls to glReadPixels().
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window_, gl_cbs::mouse_drag_cb);
  glfwSetScrollCallback(window_, gl_cbs::mouse_scroll_cb);
  glfwSetWindowUserPointer(window_, this);
  glfwSetKeyCallback(window_, key_callback);

  init_sphere_program();

  if (status_ != R_SUCCESS)
  {
    throw std::runtime_error(msg_);
  }

  init_cube_program();

  if (status_ != R_SUCCESS)
  {
    throw std::runtime_error(msg_);
  }
}

void BadEngine::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  BadEngine *me = static_cast<BadEngine *>(glfwGetWindowUserPointer(window));

  switch (key)
  {
  case GLFW_KEY_ESCAPE:
  case GLFW_KEY_ENTER:
  {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
  break;
  default:
    me->logic_key_handler_cb_(key, scancode, action, mods);
  }
}

void BadEngine::set_world_dims(glm::vec3 dims)
{
  cube_scale_ = dims;
}

BadEngine::operator bool() const
{
  return status_ == R_SUCCESS;
}

void BadEngine::draw_sphere_program(const glm::mat4 &view_trans, const glm::mat4 &projection_trans)
{
  sphere_shader_programme_.use();
  glBindVertexArray(sphere_vao_[0]);

  glm::vec3 cam_pos = cam_->get_pos();

  sphere_shader_programme_.set_mat4("view", view_trans);
  sphere_shader_programme_.set_mat4("projection", projection_trans);
  sphere_shader_programme_.set_vec3("eye_pos", cam_pos);

  for (Sphere *sphere : spheres_)
  {
    glm::mat4 model_trans(1.f);
    model_trans = glm::translate(model_trans,
                                 glm::vec3(sphere->pos.x,
                                           sphere->pos.y,
                                           sphere->pos.z));
    model_trans = glm::scale(model_trans, glm::vec3(sphere_rad_));
    sphere_shader_programme_.set_mat4("model", model_trans);
    glDrawElements(GL_TRIANGLES, (GLsizei)sphere_indices_.size(), GL_UNSIGNED_INT, NULL);
  }
}

void BadEngine::draw_cube_program(const glm::mat4 &view_trans, const glm::mat4 &projection_trans)
{
  cube_shader_programme_.use();
  glBindVertexArray(cube_vao_[0]);

  cube_shader_programme_.set_mat4("view", view_trans);
  cube_shader_programme_.set_mat4("projection", projection_trans);
  glm::mat4 model_trans(1.f);
  model_trans = glm::translate(model_trans,
                               glm::vec3(cube_.pos.x,
                                         cube_.pos.y,
                                         cube_.pos.z));
  model_trans = glm::scale(model_trans, cube_scale_);
  cube_shader_programme_.set_mat4("model", model_trans);

  glDrawElements(GL_LINE_STRIP,
                 sizeof(normalized_cube_indices) / sizeof(normalized_cube_indices[0]),
                 GL_UNSIGNED_INT,
                 NULL);
}

void BadEngine::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 view_trans = cam_->get_view();
  glm::mat4 projection_trans = cam_->get_projection();

  draw_sphere_program(view_trans, projection_trans);

  draw_cube_program(view_trans, projection_trans);

  glfwSwapBuffers(window_);
  glfwPollEvents();

  cam_->update_time_deltas();
  process_input();

#if COPY_WINDOW

  GLsizei buffer_size = screen_width_ * screen_height_ * 3;
  char *buffer = (char *)malloc(buffer_size);

  glReadPixels(0, 0, screen_width_, screen_height_, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *)buffer);

  file_copier_.Push(buffer, buffer_size);

#endif
}

bool BadEngine::loop_done() const
{
  return !this || glfwWindowShouldClose(window_);
}

void BadEngine::run()
{
  sphere_shader_programme_.use();
  glBindVertexArray(sphere_vao_[0]);

  while (!loop_done())
  {
    draw();
  }
}

void gl_cbs::mouse_drag_cb(GLFWwindow * /** Ignore */, double xpos, double ypos)
{
  if (p_camera)
    p_camera->set_cursor(xpos, ypos);
}

void gl_cbs::mouse_scroll_cb(GLFWwindow * /** Ignore */, double /** Ignore */, double y_off)
{
  if (p_camera)
    p_camera->zoom((float)y_off);
}

/** ========================================================================= */
void BadEngine::process_input()
/** ========================================================================= */
{
  cam_->process_keyboard_input();
}


void BadEngine::demo_add_spheres()
{
  add_sphere(1.f, 2.f, -.8f);
  add_sphere(-1.f, 2.f, -.8f);
}

size_t BadEngine::add_sphere(float x, float y, float z)
{
  spheres_.emplace_back(new Sphere(x, y, z, sphere_rad_));

  return spheres_.size() - 1;
}

Sphere *BadEngine::get_sphere(size_t id) const
{
  return spheres_[id];
}

void BadEngine::set_sphere_pos(int id, float x, float y, float z)
{
  spheres_[id]->pos.x = x;
  spheres_[id]->pos.y = y;
  spheres_[id]->pos.z = z;
}

void BadEngine::set_sphere_velocity(int id, float x, float y, float z)
{
  spheres_[id]->vel.x = x;
  spheres_[id]->vel.y = y;
  spheres_[id]->vel.z = z;
}

void BadEngine::set_sphere_acc(int id, float x, float y, float z)
{
  spheres_[id]->acc.x = x;
  spheres_[id]->acc.y = y;
  spheres_[id]->acc.z = z;
}

//#define APPLICATION_MODE
#ifdef APPLICATION_MODE
int main()
{
  BadEngine engine;

  engine.init();
  int status = R_SUCCESS;
  std::string msg = "";

  if (engine)
  {
    engine.run();
  }
  else
  {
    status = engine.get_status();
    msg = engine.get_message();
  }

  return utility::r_exit(status, msg);
}

#endif // APPLICATION_MODE