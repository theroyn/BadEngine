
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

  std::vector<float> sphere_data = parser_.get_data();
  std::vector<unsigned int> sphere_indices = parser_.get_indices();

  size_t sphere_count = sphere_indices.size();
  GLuint sphere_vao = 0;

  glGenVertexArrays(1, &sphere_vao);
  glBindVertexArray(sphere_vao);

  GLuint sphere_vbos[2];

  glGenBuffers(sizeof(sphere_vbos) / sizeof(sphere_vbos[0]),
               sphere_vbos);
  glBindBuffer(GL_ARRAY_BUFFER,
               sphere_vbos[0]);
  glBufferData(GL_ARRAY_BUFFER,
               sphere_data.size() * sizeof(sphere_data[0]),
               &sphere_data[0],
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_vbos[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sphere_indices.size() * sizeof(sphere_indices[0]),
               &sphere_indices[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  Shader sphere_shader_programm("base_vs.glsl", "base_fs.glsl");

  if (sphere_shader_programm.get_error())
  {
    auto msgv = sphere_shader_programm.get_message();
    msg_ = VMSG_TO_STR(msgv);
    status_ = sphere_shader_programm.get_error() ? R_FAILURE : R_SUCCESS;
  }

  RenderData render_data{ sphere_vao, sphere_count, sphere_shader_programm, true, glm::vec3(1., .5, .31) };

  render_data_.emplace(RenderableType::sphere, render_data);
}

void BadEngine::init_boxes_program()
{
  size_t box_count = sizeof(cube_coords_w_normals_n_textures) / sizeof(cube_coords_w_normals_n_textures[0]);
  GLuint box_vao = 0;
  glGenVertexArrays(1, &box_vao);
  glBindVertexArray(box_vao);

  GLuint box_vbo;

  glGenBuffers(1, &box_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, box_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(cube_coords_w_normals_n_textures),
               cube_coords_w_normals_n_textures,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0 + 3 * sizeof(GLfloat)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0 + 6 * sizeof(GLfloat)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  Shader box_shader_program = Shader("base_vs.glsl", "base_fs.glsl");

  if (box_shader_program.get_error())
  {
    auto msgv = box_shader_program.get_message();
    msg_ = VMSG_TO_STR(msgv);
    status_ = false;
  }

  RenderData render_data{ box_vao, box_count, box_shader_program, false, glm::vec3(1., .5, .71) };

  render_data_.emplace(RenderableType::box, render_data);
}

void BadEngine::init_lines_program()
{
  // normalized coordinated of 0-->1 vector
  static constexpr float VERTICES[] = {
    0.f,          //s.x
    0.f,          //s.y
    0.f,          //s.z
    0.577350259f, // e.x
    0.577350259f, // e.y
    0.577350259f  // e.z
  };

  glGenVertexArrays(1, &line_vao_);
  glBindVertexArray(line_vao_);

  GLuint line_vbo;

  glGenBuffers(1, &line_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(VERTICES),
               VERTICES,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void *>(0));

  glEnableVertexAttribArray(0);

  line_shader_programme_ = Shader("base_vs.glsl", "base_fs.glsl");

  if (line_shader_programme_.get_error())
  {
    auto msgv = line_shader_programme_.get_message();
    msg_ = VMSG_TO_STR(msgv);
    status_ = false;
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

void BadEngine::init_arrows_program()
{
  size_t arrow_count = sizeof(arrow_coords_w_normals_n_textures) / sizeof(arrow_coords_w_normals_n_textures[0]);

  GLuint arrow_vao = 0;
  GLuint arrow_vbo;

  glGenVertexArrays(1, &arrow_vao);
  glBindVertexArray(arrow_vao);


  glGenBuffers(1, &arrow_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, arrow_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(arrow_coords_w_normals_n_textures),
               arrow_coords_w_normals_n_textures,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0 + 3 * sizeof(GLfloat)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0 + 6 * sizeof(GLfloat)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  Shader arrow_shader_program("base_vs.glsl", "base_fs.glsl");

  if (arrow_shader_program.get_error())
  {
    auto msgv = arrow_shader_program.get_message();
    msg_ = VMSG_TO_STR(msgv);
    status_ = false;
  }

  RenderData render_data{ arrow_vao, arrow_count, arrow_shader_program, false, glm::vec3(0.f, 1.f, 0.f) };

  render_data_.emplace(RenderableType::arrow, render_data);
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

  init_boxes_program();

  if (status_ != R_SUCCESS)
  {
    throw std::runtime_error(msg_);
  }

  init_lines_program();

  if (status_ != R_SUCCESS)
  {
    throw std::runtime_error(msg_);
  }

  init_cube_program();

  if (status_ != R_SUCCESS)
  {
    throw std::runtime_error(msg_);
  }

  init_arrows_program();

  if (status_ != R_SUCCESS)
  {
    throw std::runtime_error(msg_);
  }
}

void BadEngine::draw_shape_program(const glm::mat4 &view_trans, const glm::mat4 &projection_trans)
{
  for (const auto &it : models_by_vao_)
  {
    const RenderData &render_data = render_data_.at(it.first);

    render_data.program.use();
    glBindVertexArray(render_data.vao);

    glm::vec3 cam_pos = cam_->get_pos();

    render_data.program.set_mat4("view", view_trans);
    render_data.program.set_mat4("projection", projection_trans);
    render_data.program.set_vec3("eye_pos", cam_pos);
    render_data.program.set_vec3("object_color", render_data.object_color);

    const std::vector<glm::mat4> &models = it.second;

    for (const glm::mat4 &model : models)
    {
      render_data.program.set_mat4("model", glm::value_ptr(model));
      if (render_data.has_element_array)
      {
        glDrawElements(GL_TRIANGLES, (GLsizei)render_data.indices_count, GL_UNSIGNED_INT, NULL);
      }
      else
      {
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)render_data.indices_count);
      }
    }
  }
}

void BadEngine::draw_lines_program(const glm::mat4 &view_trans, const glm::mat4 &projection_trans)
{
  glLineWidth(3.f);

  line_shader_programme_.use();
  glBindVertexArray(line_vao_);

  line_shader_programme_.set_mat4("view", view_trans);
  line_shader_programme_.set_mat4("projection", projection_trans);
  line_shader_programme_.set_vec3("eye_pos", cam_->get_pos());

  for (Line *line : lines_)
  {
    glm::vec3 v2 = line->end - line->start;
    glm::vec3 v2n = glm::normalize(v2);
    glm::vec3 v1 = glm::normalize(glm::vec3(1.f, 1.f, 1.f) - glm::vec3(0.f, 0.f, 0.f));
    float v2_length = glm::l2Norm(v2);

    glm::mat4 model_trans(1.f);
    float theta = acos(dot(v1, v2n));
    glm::vec3 axis = glm::normalize(glm::cross(v1, v2n));

    model_trans = glm::translate(model_trans,
                                 glm::vec3(line->start));
    model_trans = glm::rotate(model_trans, theta, axis);
    model_trans = glm::scale(model_trans, glm::vec3(v2_length));

    line_shader_programme_.set_mat4("model", model_trans);
    line_shader_programme_.set_vec3("object_color", line->color);

    glDrawArrays(GL_LINES, 0, (GLsizei)6);
  }

  glLineWidth(1.f);
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

  draw_shape_program(view_trans, projection_trans);

  draw_cube_program(view_trans, projection_trans);

  draw_lines_program(view_trans, projection_trans);

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

bool BadEngine::loop_done() const
{
  return !this || glfwWindowShouldClose(window_);
}

void BadEngine::run()
{
  render_data_.at(RenderableType::sphere).program.use();
  glBindVertexArray(render_data_.at(RenderableType::sphere).vao);

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
  add_sphere(1.f, 2.f, -.8f, false);
  add_sphere(-1.f, 2.f, -.8f, false);
}

glm::mat4 &BadEngine::get_model(RenderableType type, size_t idx)
{
  return models_by_vao_.at(type).at(idx);
}

Renderable BadEngine::add_renderable(RenderableType type)
{
  models_by_vao_[type].push_back(glm::mat4{});
  Accessor<glm::mat4> model_acc(std::bind(&BadEngine::get_model, this, type, models_by_vao_[type].size() - 1));
  return Renderable(model_acc);
}

size_t BadEngine::add_state(const glm::vec3 &pos, const glm::vec3 &vel)
{
  states_.emplace_back(pos, vel);
  return states_.size() - 1;
}

Accessor<State> BadEngine::get_state_acc(size_t idx)
{
  std::function<State &()> get_func(std::bind(&BadEngine::get_state, this, idx));
  return Accessor<State>(get_func);
}

State &BadEngine::get_state(size_t idx)
{
  return states_.at(idx);
}

size_t BadEngine::add_sphere(float x, float y, float z, bool renderable)
{
  size_t idx = add_state(glm::vec3(x, y, z), glm::vec3{});


  spheres_.push_back(new Sphere(x, y, z, sphere_rad_, get_state_acc(idx)));
  if (renderable)
  {
    Renderable r = add_renderable(RenderableType::sphere);
    spheres_[spheres_.size() - 1]->add_renderable(r);
  }

  return spheres_.size() - 1;
}

Sphere *BadEngine::get_sphere(size_t id) const
{
  return spheres_.at(id);
}

Box *BadEngine::get_box(size_t id) const
{
  return boxes_.at(id);
}

void BadEngine::set_sphere_pos(int id, float x, float y, float z)
{
  spheres_[id]->set_pos(glm::vec3(x, y, z));
}

void BadEngine::set_sphere_velocity(int id, float x, float y, float z)
{
  spheres_[id]->set_vel(glm::vec3(x, y, z));
}

size_t BadEngine::add_box(const glm::vec3 &center, const glm::vec3 &dims, bool is_static, bool renderable)
{
  size_t idx = add_state(center, glm::vec3{});
  boxes_.push_back(new Box(get_state_acc(idx), center, dims, is_static));

  if (renderable)
  {
    Renderable r = add_renderable(RenderableType::box);
    boxes_[boxes_.size() - 1]->add_renderable(r);
  }

  return boxes_.size() - 1;
}

size_t BadEngine::add_line(const glm::vec3 &start, const glm::vec3 &end)
{
  lines_.push_back(new Line(start, end));
  return lines_.size() - 1;
}

Line *BadEngine::get_line(size_t id) const
{
  return lines_.at(id);
}

size_t BadEngine::add_arrow(const glm::vec3 &pos, const glm::vec3 &dims, bool renderable)
{
  size_t idx = add_state(pos, glm::vec3{});
  arrows_.push_back(new Arrow(get_state_acc(idx), pos, dims));

  if (renderable)
  {
    Renderable r = add_renderable(RenderableType::arrow);
    arrows_[arrows_.size() - 1]->add_renderable(r);
  }
  return arrows_.size() - 1;
}

Arrow *BadEngine::get_arrow(size_t id) const
{
  return arrows_.at(id);
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