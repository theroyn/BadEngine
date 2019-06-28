#include "OBJParser.h"
#include <assert.h>
#include <algorithm>

enum InputType
{
  VERTEX_E,
  NORMAL_E,
  FACE_E,
  COMMENT_E,
  UNDEFINED_E
};

OBJParser::OBJParser()
{
}


OBJParser::~OBJParser()
{
}

bool OBJParser::parse(const std::string &path)
{
  bool success = true;
  std::ifstream fd{ path.c_str() };
  if (fd.is_open())
  {
    std::string line;
    while (std::getline(fd, line))
    {
      InputType t;
      std::string sign;
      std::stringstream s{ line };

      s >> sign;

      // ignoring 'o' and 'g'
      if (sign == "#" || sign == "" || sign == "o" || sign == "g")
      {
        t = COMMENT_E;
      }
      else if (sign == "v")
      {
        t = VERTEX_E;
      }
      else if (sign == "vn")
      {
        t = NORMAL_E;
      }
      else if (sign == "f")
      {
        t = FACE_E;
      }
      else
      {
        t = UNDEFINED_E;
        success = false;
      }
      assert(t != UNDEFINED_E);

      switch (t)
      {
      case VERTEX_E:
      {
        float f;
        std::vector<float> vec(3);
        for (int i = 0; s >> f; ++i)
        {
          assert(i < 3);
          vec[i] = f;
        }

        vertices_.push_back(std::move(vec));
      }
        break;

      case NORMAL_E:
      {
        float f;
        std::vector<float> vec(3);
        for (int i = 0; s >> f; ++i)
        {
          assert(i < 3);
          vec[i] = f;
        }

        normals_.push_back(std::move(vec));
      }
        break;

      case FACE_E:
      {
        unsigned int v, n;
        std::string str;
        std::vector<unsigned int> vec_v(3), vec_n(3);
        std::vector<std::pair<unsigned int, unsigned int>> vec_p(3);

        for (int i = 0; i < 3; ++i)
        {
          s >> str;

          std::replace(str.begin(), str.end(), '/', ' ');

          std::stringstream pair_stream(str);

          pair_stream >> v >> n;

          vec_v[i] = v;
          vec_n[i] = n;
          vec_p[i] = std::make_pair(v, n);
        }

        face_vertices_indices_.push_back(std::move(vec_v));
        face_normals_indices_.push_back(std::move(vec_v));
        face_indices_.push_back(std::move(vec_p));
      }
        break;
      case COMMENT_E:
        break;

      default:
      {
        assert(0);
        success = false;
      }

      }
    }
  }

  process();

  return success;
}
#define VERTEX_SIZE 6
void OBJParser::process()
{
  final_vertices_n_normals_.resize(vertices_.size() * VERTEX_SIZE);
  std::vector<bool> processed(vertices_.size(), false);
  unsigned int faces_n = face_vertices_indices_.size();
  indices_.reserve(faces_n * 3);
  //for each face
  for (int i = 0; i < faces_n; ++i)
  {
    auto face = face_indices_[i];
    // for each vertex in face
    for (int j = 0; j < face.size(); ++j)
    {
      auto vind = face[j].first;
      auto nind = face[j].second;
      indices_.push_back(vind - 1); // we count from 0, not from 1 like in obj files.

      if (!processed[vind-1])
      {
        processed[vind-1] = true;

        final_vertices_n_normals_[((vind - 1) * VERTEX_SIZE) + 0] = vertices_[vind - 1][0];
        final_vertices_n_normals_[((vind - 1) * VERTEX_SIZE) + 1] = vertices_[vind - 1][1];
        final_vertices_n_normals_[((vind - 1) * VERTEX_SIZE) + 2] = vertices_[vind - 1][2];

        final_vertices_n_normals_[((vind - 1) * VERTEX_SIZE) + 3] = normals_[nind - 1][0];
        final_vertices_n_normals_[((vind - 1) * VERTEX_SIZE) + 4] = normals_[nind - 1][1];
        final_vertices_n_normals_[((vind - 1) * VERTEX_SIZE) + 5] = normals_[nind - 1][2];
      }
    }

  }

}