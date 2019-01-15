#pragma once
#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
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


  void initObject(int index,glmlv::SimpleGeometry &obj);
  void draw(int index,GLsizei size);
  
  glmlv::SimpleGeometry cube;
  glm::mat4 cubeModel;
  glmlv::SimpleGeometry sphere;
  glm::mat4 sphereModel;
  GLuint vbo[2];
  GLuint vao[2];
  GLuint ibo[2];
  //Shader part
  glmlv::GLProgram shader;
  GLint uMVP, uMV, uNormal;
  glm::mat4 projection;
  glm::mat4 view;
  float f = 0;
};
