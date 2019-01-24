#pragma once
#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace std;
using namespace glm;

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

  void initObject(int index,glmlv::SimpleGeometry &obj);
  void draw(int index,GLsizei size);
  
  glmlv::SimpleGeometry cube;
  glm::mat4 cubeModel;
  glm::vec4 cubeColor;

  
  glmlv::SimpleGeometry sphere;
  glm::mat4 sphereModel;
  glm::vec4 sphereColor;

  //GL
  GLuint vbo[2];
  GLuint vao[2];
  GLuint ibo[2];
  GLuint texture[2];
  glmlv::Image2DRGBA b_textures[2];
  //Shader part
  glmlv::GLProgram shader;
  GLint uMVP, uMV, uNormal;
  GLint uDirectionalLightDir,uDirectionalLightIntensity, uPointLightPosition, uPointLightIntensity, uKd;
  vec3 dirLightPosition = vec3(0,0,0);
  vec3 pointLightPosition = vec3(0,0,0);
  glm::mat4 projection;
  glm::mat4 view;
  float f = 0;
  glmlv::ViewController *viewC;

  //GUI
  float g_cubeColor[4] = {0,0,0,1},g_sphereColor[4]= {0,0,0,1};
  float g_dirLightPosition[3] = {0,0,0},g_pointLightPosition[3]= {0,0,0};
  float g_dirLightIntensity = 0,g_pointLightIntensity = 0;
};
