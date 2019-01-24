#include "Application.hpp"

#include <iostream>



int Application::run()
{
  // Put here code to run before rendering loop

  shader.use();

  // Loop until the user closes the window
  for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
      
      glBindTexture(GL_TEXTURE_2D, texture[0]);
      glTexSubImage2D(GL_TEXTURE_2D,
		      0,0,0,
		      b_textures[0].width(),
		      b_textures[0].height(),
		      GL_RGBA,GL_UNSIGNED_BYTE,images[im].data());
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindTexture(GL_TEXTURE_2D, texture[1]);
      glTexSubImage2D(GL_TEXTURE_2D,
		      0,0,0,
		      b_textures[0].width(),
		      b_textures[0].height(),
		      GL_RGBA,GL_UNSIGNED_BYTE,images[im].data());
      glBindTexture(GL_TEXTURE_2D, 0);
      ++im;
      if(im == images.size())
	im = 0;
      const auto seconds = glfwGetTime();
      //LIght init
      glUniform3fv(uDirectionalLightDir, 1, value_ptr(dirLightPosition));
      glUniform3fv(uDirectionalLightIntensity, 1, value_ptr(vec3(g_dirLightIntensity,g_dirLightIntensity,g_dirLightIntensity)));
      glUniform3fv(uPointLightPosition, 1, value_ptr(pointLightPosition));
      glUniform3fv(uPointLightIntensity, 1, value_ptr(vec3(g_pointLightIntensity,g_pointLightIntensity,g_pointLightIntensity)));
      // Put here rendering code
      const auto fbSize = m_GLFWHandle.framebufferSize();
      glViewport(0, 0, fbSize.x, fbSize.y);
      glClearColor(1,1,1,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
      //Uniform update
      view = translate(viewC->getViewMatrix(),vec3(2,0,0)) ;
      f+=0.1;
      cubeModel = glm::rotate(glm::mat4(1.f),radians(f),vec3(0,1,1));
      cubeModel = scale(cubeModel,vec3(1.5,1.5,1.5));
      mat4 mvp =  projection * view * cubeModel ;
      glUniformMatrix4fv(uMVP, 1, GL_FALSE, &mvp[0][0]);

      mat4 mv =  cubeModel;
      glUniformMatrix4fv(uMV, 1, GL_FALSE, &mv[0][0]);

      mat4 normalM = glm::transpose(glm::inverse(mv));
      glUniformMatrix4fv(uNormal, 1, GL_FALSE, &normalM[0][0]);

      //Cube color
      /* glUniform1i(uKdSampler, 0);
      glBindSampler(0, sampler[0]);
      glBindTexture(GL_TEXTURE_2D, texture[0]);
      glUniform3fv(uKd, 1, value_ptr(vec3(cubeColor)));
      draw(0,cube.indexBuffer.size());
      glBindTexture(GL_TEXTURE_2D, 0);*/
      //Draw Sphere
      mvp =  projection * view * sphereModel ;
      glUniformMatrix4fv(uMVP, 1, GL_FALSE, &mvp[0][0]);

      mv =  sphereModel;
      glUniformMatrix4fv(uMV, 1, GL_FALSE, &mv[0][0]);

      normalM = glm::transpose(glm::inverse(mv));
      glUniformMatrix4fv(uNormal, 1, GL_FALSE, &normalM[0][0]);

      glUniform1i(uKdSampler, 0);
      glBindSampler(0, sampler[1]);
      glBindTexture(GL_TEXTURE_2D, texture[1]);
      glUniform3fv(uKd, 1,  value_ptr(vec3(sphereColor)));
      draw(1,sphere.indexBuffer.size());
      glBindTexture(GL_TEXTURE_2D, 0);
      

      // GUI code:
      glmlv::imguiNewFrame();

      {
	ImGui::Begin("GUI");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::ColorEdit4("Sphere Color",g_cubeColor);
	for(int i = 0; i < 3; ++i)
	  sphereColor[i] = g_cubeColor[i]; 
	ImGui::ColorEdit4("Cube Color",g_sphereColor);
	for(int i = 0; i < 3; ++i)
	  cubeColor[i] = g_sphereColor[i];
	
	ImGui::InputFloat3("Directional Light Position",g_dirLightPosition);
	for(int i = 0; i < 3; ++i)
	  dirLightPosition[i] = g_dirLightPosition[i];
	ImGui::SliderFloat("Directional Light Intensity", &g_dirLightIntensity, 0.0f, 1.0f);
	ImGui::InputFloat3("Point Light Position",g_pointLightPosition);
	for(int i = 0; i < 3; ++i)
	  pointLightPosition[i] = g_pointLightPosition[i];
	ImGui::SliderFloat("Point Light Intensity", &g_pointLightIntensity, 0.0f, 100.0f);

	ImGui::End();
      }

      glmlv::imguiRenderFrame();

      glfwPollEvents(); // Poll for and process events

      auto ellapsedTime = glfwGetTime() - seconds;
      auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
      if (!guiHasFocus) {
	// Put here code to handle user interactions
      }
      while(ellapsedTime < (float)1/60){
	ellapsedTime = glfwGetTime() - seconds;

      }
      viewC->update(ellapsedTime);
      m_GLFWHandle.swapBuffers(); // Swap front and back buffers
    }

  return 0;
}

Application::Application(int argc, char** argv):
  m_AppPath { glmlv::fs::path{ argv[0] } },
  m_AppName { m_AppPath.stem().string() },
  m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
  m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
  m_AssetsRootPath { m_AppPath.parent_path() / "assets" }

{

  ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

  //Shaders init
  shader = glmlv::compileProgram({m_ShadersRootPath / m_AppName / "forward.vs.glsl" ,
	m_ShadersRootPath / m_AppName / "forward.fs.glsl" });

  uMVP = shader.getUniformLocation("uModelViewProjMatrix");
  uMV = shader.getUniformLocation("uModelViewMatrix");
  uNormal = shader.getUniformLocation("uNormalMatrix");
  //Lighting unfiroms init
  uDirectionalLightDir = shader.getUniformLocation("uDirectionalLightDir") ;
  uDirectionalLightIntensity = shader.getUniformLocation("uDirectionalLightIntensity");
  uPointLightPosition = shader.getUniformLocation("uPointLightPosition");
  uPointLightIntensity = shader.getUniformLocation("uPointLightIntensity");
  uKd = shader.getUniformLocation("uKd");
  uKdSampler  = shader.getUniformLocation("uKdSampler");

  //Init matrices
  projection = glm::perspective(glm::radians(70.f),(float)m_nWindowWidth/m_nWindowHeight,0.1f,100.f);
  view = lookAt(vec3(5,0,0),vec3(0,0,0),vec3(0,1,0));
  //view = glm::ortho(-1,1,1,1);
  // Put here initialization code
  glEnable(GL_DEPTH_TEST);
  cube = glmlv::makeCube();
  sphere = glmlv::makeSphere(32);

  cout << "Content created" << endl;
  //Init des buffers 
  glGenBuffers(2,vbo);
  glGenBuffers(2,ibo);
  glGenVertexArrays(2,vao);
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(2,texture);
  glGenSamplers(2,sampler);
  //Init des objets
  b_textures[0] = glmlv::readImage({m_AssetsRootPath / m_AppName / "fire_0000.png"});
  initObject(0,cube);
  cubeModel = glm::rotate(glm::mat4(1.f),radians(45.f),vec3(0,1,0));
  cubeColor = vec4(0,1,0,1);

  b_textures[1] = glmlv::readImage({m_AssetsRootPath / m_AppName / "fire_0000.png"});
  initObject(1,sphere);
  sphereModel = glm::rotate(glm::mat4(1.f),radians(45.f),vec3(0,1,0));
  sphereColor = vec4(1,0,0,1);
  //LOAD IMAGES
  for(int i = 0; i < 46; ++i){
    char buffer[9];
    std::snprintf(buffer, sizeof(buffer), "%04d", i);
    string s = string("fire_") + string(buffer) + string(".png");
    images.push_back(glmlv::readImage({m_AssetsRootPath / m_AppName  /s}));
  }

  //CAMERA VIEW HANDLE
  viewC = new glmlv::ViewController( m_GLFWHandle.window(),30);
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
  //texture
  glBindTexture(GL_TEXTURE_2D, texture[index]) ;
  glTexStorage2D( GL_TEXTURE_2D,
        1,
  	 GL_RGBA8,
  	b_textures[index].width(),
  	b_textures[index].height());
  glTexSubImage2D(GL_TEXTURE_2D,
		      0,0,0,
		      b_textures[index].width(),
		      b_textures[index].height(),
		      GL_RGBA,GL_UNSIGNED_BYTE,b_textures[index].data());
  glSamplerParameteri(sampler[index],
 	GL_TEXTURE_MIN_FILTER,
 	GL_LINEAR);
  glSamplerParameteri(sampler[index],
 	GL_TEXTURE_MAG_FILTER,
 	GL_LINEAR);
  
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
