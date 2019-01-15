#include "Application.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Application::run()
{
  // Put here code to run before rendering loop

  shader.use();
  // Loop until the user closes the window
  for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
      const auto seconds = glfwGetTime();

      // Put here rendering code
      const auto fbSize = m_GLFWHandle.framebufferSize();
      glViewport(0, 0, fbSize.x, fbSize.y);
      glClearColor(0,0,0,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      draw(0,cube.vertexBuffer.size());
      //draw(1,sphere.vertexBuffer.size());
      

      // GUI code:
      glmlv::imguiNewFrame();

      {
	ImGui::Begin("GUI");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
      }

      glmlv::imguiRenderFrame();

      glfwPollEvents(); // Poll for and process events

      auto ellapsedTime = glfwGetTime() - seconds;
      auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
      if (!guiHasFocus) {
	// Put here code to handle user interactions
      }

      m_GLFWHandle.swapBuffers(); // Swap front and back buffers
    }

  return 0;
}

Application::Application(int argc, char** argv):
  m_AppPath { glmlv::fs::path{ argv[0] } },
  m_AppName { m_AppPath.stem().string() },
  m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
  m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }

{

  ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

  //Shaders init
  shader = glmlv::compileProgram({m_ShadersRootPath / m_AppName / "forward.vs.glsl" ,
	m_ShadersRootPath / m_AppName / "forward.fs.glsl" });

  uMVP = shader.getUniformLocation("uModelViewProjMatrix");
  uMV = shader.getUniformLocation("uModelViewMatrix");
  uNormal = shader.getUniformLocation("uNormalMatrix");
  // Put here initialization code
  glEnable(GL_DEPTH_TEST);
  cube = glmlv::makeCube();
  sphere = glmlv::makeSphere(10);

  cout << "Content created" << endl;
  
  glGenBuffers(2,vbo);
  glGenBuffers(2,ibo);
  glGenVertexArrays(2,vao);
  
  initObject(0,cube);
  initObject(1,sphere);

}

void Application::initObject(int index,glmlv::SimpleGeometry &obj){
  
  //Init cube
  //VBO
  cout << "VBO init" << endl;
  glBindBuffer(GL_ARRAY_BUFFER,vbo[index]);

  glBufferStorage(GL_ARRAY_BUFFER,obj.vertexBuffer.size()*sizeof(glmlv::Vertex3f3f2f),obj.vertexBuffer.data(),GL_DYNAMIC_STORAGE_BIT);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  cout << "VBO DOne" <<endl;
  
  //IBO
  cout << "IBO Init" << endl;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo[index]);
  glBufferStorage(GL_ELEMENT_ARRAY_BUFFER,obj.indexBuffer.size()*sizeof(uint32_t),obj.indexBuffer.data(),GL_DYNAMIC_STORAGE_BIT);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
  cout << "IBO done"  << endl;
  
  //VAO
  cout << "VAO init" << endl;

  glBindVertexArray(vao[index]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[index]);
  glBindVertexBuffer(0,vbo[index],0,sizeof(glmlv::Vertex3f3f2f));
  
  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
  glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(glmlv::Vertex3f3f2f, position));
  glVertexAttribBinding(VERTEX_ATTR_POSITION, 0);
  
  glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
  glVertexAttribFormat(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(glmlv::Vertex3f3f2f, normal));
  glVertexAttribBinding(VERTEX_ATTR_NORMAL, 0);
  
  glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE);
  glVertexAttribFormat(VERTEX_ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, offsetof(glmlv::Vertex3f3f2f, texCoords));
  glVertexAttribBinding(VERTEX_ATTR_TEXTURE, 0);
  
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  cout << "VAO done" << endl;
}

void Application::draw(int index,GLsizei size){
  glBindVertexArray(vao[index]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[index]);
  glDrawElements(GL_TRIANGLES, size , GL_UNSIGNED_INT,0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
