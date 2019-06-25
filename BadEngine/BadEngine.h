#pragma once

#include <string>
#include "Camera.h"
#include "const.h"


class BadEngine
{
public:
  BadEngine();
  ~BadEngine();

public:
  void init();
  void run();
  operator bool() const;

public:
  int get_status() const { return status_; }
  std::string get_message() const { return msg_; }

private:
  void process_input();

private:
  GLFWwindow *window_;
  int status_;
  std::string msg_;
  Camera *cam_;
};

