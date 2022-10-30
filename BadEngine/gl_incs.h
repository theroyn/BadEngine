#pragma once

/**
 * The following order is crucial:
 * 1. glew.h
 * 2. freeglut.h
 * 3. GLFW_DLL
 * 4. glfw3.h
 */

#include "GL/glew.h"

#include <GL/freeglut.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
