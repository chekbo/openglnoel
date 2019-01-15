#include "Application.hpp"

#include <iostream>



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
      glClearColor(1,1,1,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
      //Uniform update
      f+=0.1;
      cubeModel = glm::rotate(glm::mat4(1.f),radians(f),vec3(0,1,1));
      mat4 mvp =  projection * view * cubeModel ;
      glUniformMatrix4fv(uMVP, 1, GL_FALSE, &mvp[0][0]);

      mat4 mv = view * cubeModel;
      glUniformMatrix4fv(uMV, 1, GL_FALSE, &mv[0][0]);

      mat4 normalM = glm::transpose(glm::inverse(mv));
      glUniformMatrix4fv(uNormal, 1, GL_FALSE, &normalM[0][0]);

      draw(0,cube.vertexBuffer.size());

      //Draw Sphere
      mvp =  projection * view * sphereModel ;
      glUniformMatrix4fv(uMVP, 1, GL_FALSE, &mvp[0][0]);

      mv = view * sphereModel;
      glUniformMatrix4fv(uMV, 1, GL_FALSE, &mv[0][0]);

      normalM = glm::transpose(glm::inverse(mv));
      glUniformMatrix4fv(uNormal, 1, GL_FALSE, &normalM[0][0]);
      draw(1,sphere.vertexBuffer.size());
      

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

  //Init matrices
  projection = glm::perspective(glm::radians(70.f),(float)m_nWindowWidth/m_nWindowHeight,0.1f,100.f);
  view = lookAt(vec3(5,0,0),vec3(0,0,0),vec3(0,1,0));
  //view = glm::ortho(-1,1,1,1);
  // Put here initialization code
  glEnable(GL_DEPTH_TEST);
  cube = glmlv::makeCube();
  sphere = glmlv::makeSphere(10);

  cout << "Content created" << endl;
  //Init des buffers 
  glGenBuffers(2,vbo);
  glGenBuffers(2,ibo);
  glGenVertexArrays(2,vao);

  //Init des objets
  initObject(0,cube);
  cubeModel = glm::rotate(glm::mat4(1.f),radians(45.f),vec3(0,1,0));
  
  initObject(1,sphere);
  sphereModel = glm::rotate(glm::mat4(1.f),radians(45.f),vec3(0,1,0));
  

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
  glBufferStorage(GL_ELEMENT_ARRAY_BUFFER,obj.vertexBuffer.size()*sizeof(uint32_t),obj.indexBuffer.data(),GL_DYNAMIC_STORAGE_BIT);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
  cout << "IBO done"  << endl;
  
  //VAO
  cout << "VAO init" << endl;

  glBindVertexArray(vao[index]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[index]);
  glBindVertexBuffer(0,vbo[index],0,sizeof(glmlv::Vertex3f3f2f));
  
  glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
  glVertexAttribFormat(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, offsetof(glmlv::Vertex3f3f2f, position));
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
