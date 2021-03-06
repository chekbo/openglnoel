#pragma once

//glmlv
#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//std
#include <iostream>
#include <vector>
#include <map>

#include "tiny_gltf.h"
//Namespaces
using namespace std;
using namespace glm;
using namespace tinygltf;

//consts
const GLint VERTEX_ATTR_POSITION = 0;
const GLint VERTEX_ATTR_NORMAL = 1;
const GLint VERTEX_ATTR_TEXTURE = 2;


class Application
{
public:
  Application(int argc, char** argv);

  int run();
private:
  const size_t m_nWindowWidth = 1280;
  const size_t m_nWindowHeight = 720;
  glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

  const glmlv::fs::path m_AppPath;
  const std::string m_AppName;
  const std::string m_ImGuiIniFilename;
  const glmlv::fs::path m_ShadersRootPath;
  const glmlv::fs::path m_AssetsRootPath;

  Model model_File;
  
  void draw(Model &model);
  void drawNode(Model &model, Node &node);
  void drawMesh(Model &model, Mesh &mesh);
  void initObject(Model &obj);
  void bindModelNodes(std::map<int, GLuint> vbos, tinygltf::Model &model, tinygltf::Node &node);
  std::map<int, GLuint>  bindMesh(std::map<int, GLuint> vbos,tinygltf::Model &model, tinygltf::Mesh &mesh);
  glmlv::SimpleGeometry cube;
  glm::mat4 cubeModel;
  glm::vec4 cubeColor;

  
  glmlv::SimpleGeometry sphere;
  glm::mat4 sphereModel;
  glm::vec4 sphereColor;

  //GLBuffers
  std::map<int, GLuint> vbos;
  vector<GLuint> vaos;
  vector<GLuint> ibos;
  vector<GLuint> textures;
  vector<GLuint> samplers;
  GLuint vbo[2];
  GLuint vao[2];
  GLuint ibo[2];
  GLuint texture[2];
  GLuint sampler[2];
  glmlv::Image2DRGBA b_textures[2];
  
  //Shader part
  glmlv::GLProgram shader;
  GLint uMVP, uMV, uNormal;
  GLint uDirectionalLightDir,uDirectionalLightIntensity, uPointLightPosition, uPointLightIntensity, uKd;
  GLint uKdSampler;
  vec3 dirLightPosition = vec3(0,0,0);
  vec3 pointLightPosition = vec3(0,0,0);
  glm::mat4 projection;
  glm::mat4 view;
  float f = 0;
  glmlv::ViewController *viewC;

  //GUI
  float g_cubeColor[4] = {1,1,1,1},g_sphereColor[4]= {1,1,1,1};
  float g_dirLightPosition[3] = {1,1,1},g_pointLightPosition[3]= {1,1,1};
  float g_dirLightIntensity = 1,g_pointLightIntensity = 1;

  
  int im = 0;
  std::vector<glmlv::Image2DRGBA> images;
};
