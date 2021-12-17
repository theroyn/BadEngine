
#include "FileCopier.h"

FileCopier::FileCopier()
{
  Init();
}
FileCopier::~FileCopier()
{
  if (t_.joinable())
  {
    isStopped = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    ClearBuffers();
    t_.join();
  }
}

void FileCopier::Push(char *buf, int buffer_size)
{
  if (!isStopped)
  {
    buffers_.push_back(Buffer(buf, buffer_size));
  }
}

void FileCopier::ClearBuffers()
{
  while (!buffers_.empty())
  {
    Buffer curr = buffers_.front();
    buffers_.pop_front();
    free(curr.buffer);
  }
  for (Buffer &buf : buffers_)
  {
    free(buf.buffer);
  }
  buffers_.clear();
}

void FileCopier::Init()
{
  t_ = std::thread(std::bind(&FileCopier::Copy, this));
}

void FileCopier::Copy()
{
  while (!isStopped)
  {
    while (!buffers_.empty())
    {
      Buffer curr = buffers_.front();
      buffers_.pop_front();

      std::string file_counter_str = std::to_string(file_counter_++);

      int gap = 5 - file_counter_str.size();
      gap = (gap < 0) ? 0 : gap;
      for (int i = 0; i < gap; ++i)
      {
        file_counter_str = "0" + file_counter_str;
      }

      std::fstream fd("D:\\tmp\\file" + file_counter_str + ".raw", std::ios::out | std::ios::binary);

      fd.write(curr.buffer, curr.buffer_size);
      fd.close();
      free(curr.buffer);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  }
}