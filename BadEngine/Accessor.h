#pragma once

#include "gl_incs.h"

#include <functional>

template <class T>
class Accessor
{
public:
  Accessor(std::function<T &()> get_func) : get_func_(get_func) {}
  T get() const
  {
    return get_func_();
  }
  T& get()
  {
    return get_func_();
  }
  void set(const T &v)
  {
    get_func_() = v;
  }

private:
  std::function<T &()> get_func_;
};