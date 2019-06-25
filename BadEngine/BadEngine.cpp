
#include <iostream>
#include <sstream>

#include "gl_incs.h"

#include "utils.h"
#include "BadEngine.h"
#include "coords.h"
#include "Shader.h"

const GLuint SCR_WIDTH = 800;
const GLuint SCR_HEIGHT = 600;

namespace gl_cbs
{
  static Camera *p_camera = nullptr;
  void mouse_drag_cb(GLFWwindow * /** Ignore */, double xpos, double ypos);
  void mouse_scroll_cb(GLFWwindow * /** Ignore */, double /** Ignore */, double y_off);
}

#define VMSG_TO_STR(msg) &((msg)[0])

void framebuffer_size_cb(GLFWwindow* window, int width, int height);


#define APPLICATION_MODE
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


void framebuffer_size_cb(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

BadEngine::BadEngine() : status_(R_SUCCESS), msg_(""), cam_(nullptr)
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
}

void BadEngine::init()
{
  if (glfwInit() == GL_TRUE)
  {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello Texture", NULL, NULL);

    if (window_ != NULL)
    {
      glfwMakeContextCurrent(window_);
      glfwSetFramebufferSizeCallback(window_, framebuffer_size_cb);

      // start GLEW extension handler
      glewExperimental = GL_TRUE;
      int res = glewInit();

      const GLubyte* renderer = glGetString(GL_RENDERER);
      const GLubyte* version = glGetString(GL_VERSION);

      std::stringstream inf;
      inf << "Renderer:" << renderer << std::endl << "OpenGL version supported:" << version << std::endl;
      utility::dbg_print(inf.str());

      glm::vec3 camera_pos(0.f, 0.f, 3.f);
      glm::vec3 camera_front(0.f, 0.f, -1.f);
      glm::vec3 world_up(0.f, 1.f, 0.f);

      cam_ = new Camera(window_, camera_pos, camera_front, world_up);
    }
    else
    {
      status_ = -1;
      msg_ = "Cannot create opengl window.";
    }
  }
  else
  {
    status_ = -1;
    msg_ = "Cannot initiate opengl.";
  }

  gl_cbs::p_camera = cam_;
}

BadEngine::operator bool() const
{
  return status_ == 0;
}

void BadEngine::run()
{
  glEnable(GL_DEPTH_TEST);

  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window_, gl_cbs::mouse_drag_cb);
  glfwSetScrollCallback(window_, gl_cbs::mouse_scroll_cb);

  /*GLuint vao[] = { 0 };
  glGenVertexArrays(1, vao);
  glBindVertexArray(vao[0]);

  GLuint vbo[] = { 0 };
  glGenBuffers(sizeof(vbo) / sizeof(vbo[0]), vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_coords_w_textures), cube_coords_w_textures, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast< void * >(0));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast< void * >(3 * sizeof(GLfloat)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
*/

  GLuint vbo[] = { 0 };
  glGenBuffers(sizeof(vbo) / sizeof(vbo[0]), vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(cube_coords_w_normals_n_textures),
               cube_coords_w_normals_n_textures,
               GL_STATIC_DRAW);
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);


  // Lamp vertices.
  glBindVertexArray(vao);

  glVertexAttribPointer(0, 3,
                        GL_FLOAT, GL_FALSE,
                        8 * sizeof(GLfloat), reinterpret_cast< void * >(0));
  glEnableVertexAttribArray(0);

  Shader shader_programme = Shader("base_vs.glsl", "base_fs.glsl");

  if (shader_programme.get_error())
  {
    auto msgv = shader_programme.get_message();
    msg_ = VMSG_TO_STR(msgv);
    status_ = shader_programme.get_status();
  }

  shader_programme.use();
  glBindVertexArray(vao);

  glm::mat4 view_trans = cam_->get_view();
  glm::mat4 projection_trans = cam_->get_projection();

  while (*this && !glfwWindowShouldClose(window_))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_programme.use();


    glBindVertexArray(vao);
    shader_programme.set_mat4("view", view_trans);
    shader_programme.set_mat4("projection", projection_trans);

    glm::mat4 model_trans;
    model_trans = glm::translate(model_trans, cube_positions[0]);
    shader_programme.set_mat4("model", model_trans);
    //glUniformMatrix4fv(model_trans_loc, 1, GL_FALSE, glm::value_ptr(model_trans));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    /*glm::mat4 model_trans = glm::translate(glm::mat4(), cube_positions[0]);

    shader_programme.set_mat4("model", model_trans);

    glDrawArrays(GL_TRIANGLES, 0, 36);*/

    glfwSwapBuffers(window_);
    glfwPollEvents();

    cam_->update_time_deltas();
    process_input();
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
    p_camera->zoom(y_off);
}

/** ========================================================================= */
void BadEngine::process_input()
/** ========================================================================= */
{
  if (GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_ESCAPE) ||
      GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_ENTER))
    glfwSetWindowShouldClose(window_, GL_TRUE);

  cam_->process_keyboard_input();
}