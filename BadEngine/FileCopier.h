#pragma once

#include <deque>
#include <fstream>
#include <functional>
#include <string>
#include <thread>

class FileCopier
{
public:
  FileCopier();
  ~FileCopier();
  void Push(char *buf, int buffer_size);

private:
  struct Buffer
  {
    Buffer(char *buffer, int buffer_size) : buffer(buffer), buffer_size(buffer_size) {}

    char *buffer;
    int buffer_size;
  };
  void ClearBuffers();
  void Init();

  void Copy();

private:
  std::thread t_;
  std::fstream fd_;
  std::deque<Buffer> buffers_;
  bool isStopped = false;
  size_t file_counter_ = 0;
};
