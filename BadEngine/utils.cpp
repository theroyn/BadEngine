#include "utils.h"
#include <iostream>

#include "gl_incs.h"

#include <fstream>

using namespace std;


void utility::dbg_print(const string &msg)
{
#ifdef _DEBUG
  cout << "Debug Message:" << endl << msg << endl;
#endif // DEBUG
}

int utility::r_exit(int code, const string &msg)
{
  if (!msg.empty())
    dbg_print(msg);

  glfwTerminate();

#ifdef _DEBUG
  if (code != 0)
  {
    cout << endl << "Press Enter to exit with code " << code << "..." << endl;

    cin.clear();
    cin.get();
  }
#endif // DEBUG


  return code;
}

string utility::read_file(const char *file_path)
{
  string content;
  ifstream file_stream(file_path, ios::in);

  if (!file_stream.is_open())
  {
    cerr << "Could not read file " << file_path << ". File does not exist." << endl;
    return "";
  }

  string line = "";
  while (!file_stream.eof())
  {
    getline(file_stream, line);
    content.append(line + "\n");
  }

  file_stream.close();
  return content;
}