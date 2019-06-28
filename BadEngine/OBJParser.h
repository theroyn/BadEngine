#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <utility>

//struct face
//{
//  unsigned int sz;
//  float *verts;
//  float *verts_normals;
//};

class OBJParser
{
public:
  OBJParser();
  ~OBJParser();

public:
  bool parse(const std::string &path);
  std::vector<std::vector<float>> get_vertices() const { return vertices_; }
  std::vector<std::vector<unsigned int>> get_face_indices() const { return face_vertices_indices_; }
  std::vector<std::vector<float>> get_normals() const { return normals_; }
  std::vector<std::vector<unsigned int>> get_face_normal_indices() const { return face_normals_indices_; }
  std::vector<float> get_data() const { return final_vertices_n_normals_; }
  std::vector<unsigned int> get_indices() const { return indices_; }

private:
  void process();

private:
  std::vector<std::vector<float>> vertices_;
  std::vector<std::vector<float>> normals_;
  std::vector<std::vector<unsigned int>> face_vertices_indices_;
  std::vector<std::vector<unsigned int>> face_normals_indices_;
  std::vector<std::vector<std::pair<unsigned int, unsigned int>>> face_indices_;

  std::vector<float> final_vertices_n_normals_;
  std::vector<unsigned int> indices_;
};


