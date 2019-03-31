#include "Application.hpp"
#define TINYGLTF_IMPLEMENTATION
//third-party
#include "tiny_gltf.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

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
     
      draw(model_File);
      

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
  view = lookAt(vec3(5,2,5),vec3(0,0,0),vec3(0,1,0));
  //view = glm::ortho(-1,1,1,1);
  
  // Put here initialization code
  glEnable(GL_DEPTH_TEST);

  
  //Init des buffers 

  
  //Init des objets
  /**
     Loading gltf
   */
  TinyGLTF loader;
  string err,warn;
  bool ret = loader.LoadASCIIFromFile(&model_File,&err,&warn,argv[1]);
  if(!warn.empty())
    cout << "WARNING : " << warn << endl;
  if(ret == 0){
    cout << "ERROR : " << err << endl;
    exit(-1);
  }
  
  initObject(model_File);
  
  //LOAD IMAGES


  //CAMERA VIEW HANDLE
  viewC = new glmlv::ViewController( m_GLFWHandle.window(),30);
}

void Application::initObject(Model &model){
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    bindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
  }
 
  glBindVertexArray(0);
}

// bind models
void Application::bindModelNodes(std::map<int, GLuint> vbos, tinygltf::Model &model,
                    tinygltf::Node &node) {
  bindMesh(vbos, model, model.meshes[node.mesh]);
  for (size_t i = 0; i < node.children.size(); i++) {
    bindModelNodes(vbos, model, model.nodes[node.children[i]]);
         
  }
}


std::map<int, GLuint> Application::bindMesh(std::map<int, GLuint> vbos,
                               tinygltf::Model &model, tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == 0) {  // TODO impl drawarrays
      std::cout << "WARN: bufferView.target is zero" << std::endl;
      continue;  // Unsupported bufferView.
                 /*
                   From spec2.0 readme:
                   https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
                            ... drawArrays function should be used with a count equal to
                   the count            property of any of the accessors referenced by the
                   attributes            property            (they are all equal for a given
                   primitive).
                 */
    }

    tinygltf::Buffer buffer = model.buffers[bufferView.buffer];
    std::cout << "bufferview.target " << bufferView.target << std::endl;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    vbos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);

    std::cout << "buffer.data.size = " << buffer.data.size()
              << ", bufferview.byteOffset = " << bufferView.byteOffset
              << std::endl;

    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
    for (auto &attrib : primitive.attributes) {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byteStride =
          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
      glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }
      
      int vaa = -1;
      if (attrib.first.compare("POSITION") == 0) vaa = 0;
      if (attrib.first.compare("NORMAL") == 0) vaa = 1;
      if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
      if (vaa > -1) {
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(vaa, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, BUFFER_OFFSET(accessor.byteOffset));
      } else
        std::cout << "vaa missing: " << attrib.first << std::endl;
    }

    /*GLuint texid;
    glGenTextures(1, &texid);

    tinygltf::Texture &tex = model.textures[0];
    tinygltf::Image &image = model.images[tex.source];

    glBindTexture(GL_TEXTURE_2D, texid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum format = GL_RGBA;

    if (image.component == 1) {
      format = GL_RED;
    } else if (image.component == 2) {
      format = GL_RG;
    } else if (image.component == 3) {
      format = GL_RGB;
    } else {
      // ???
    }

    GLenum type = GL_UNSIGNED_BYTE;
    if (image.bits == 8) {
      // ok
    } else if (image.bits == 16) {
      type = GL_UNSIGNED_SHORT;
    } else {
      // ???
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
    format, type, &image.image.at(0));*/
  }
    
  return vbos;
}

void Application::draw(Model &model){
  /*glBindVertexArray(vao[index]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[index]);
  glDrawElements(GL_TRIANGLES, size , GL_UNSIGNED_INT,0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);*/
  int scene_to_display = model.defaultScene > -1 ? model.defaultScene : 0;
  const tinygltf::Scene &scene = model.scenes[scene_to_display];
  for (size_t i = 0; i < scene.nodes.size(); i++) {
    drawNode(model, model.nodes[scene.nodes[i]]);
  }

}

void Application::drawNode(Model &model, Node &node){
  if (node.mesh > -1) {
    assert(node.mesh < model.meshes.size());
    drawMesh(model, model.meshes[node.mesh]);
  }

  // Draw child nodes.
  for (size_t i = 0; i < node.children.size(); i++) {
    assert(node.children[i] < model.nodes.size());
    drawNode(model, model.nodes[node.children[i]]);
  }
}

void Application::drawMesh(Model &model, Mesh &mesh){
  for (size_t i = 0; i < mesh.primitives.size(); i++) {
    const tinygltf::Primitive &primitive = mesh.primitives[i];

    if (primitive.indices < 0) return;

    // Assume TEXTURE_2D target for the texture object.
    // glBindTexture(GL_TEXTURE_2D, gMeshState[mesh.name].diffuseTex[i]);

    std::map<std::string, int>::const_iterator it(primitive.attributes.begin());
    std::map<std::string, int>::const_iterator itEnd(
						     primitive.attributes.end());

    for (; it != itEnd; it++) {

      const tinygltf::Accessor &indexAccessor =
	model.accessors[primitive.indices];
      
      glBindVertexArray(vaos[indexAccessor.bufferView]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
		   vbos[indexAccessor.bufferView]);

      /**Drawing part*/
      view = translate(viewC->getViewMatrix(),vec3(2,0,0)) ;
      cubeModel = glm::rotate(glm::mat4(1.f),0.0f,vec3(0,0,0));
      mat4 mvp =  projection * view * cubeModel ;
      glUniformMatrix4fv(uMVP, 1, GL_FALSE, &mvp[0][0]);

      mat4 mv =  cubeModel;
      glUniformMatrix4fv(uMV, 1, GL_FALSE, &mv[0][0]);

      mat4 normalM = glm::transpose(glm::inverse(mv));
      glUniformMatrix4fv(uNormal, 1, GL_FALSE, &normalM[0][0]);
      
      glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
	BUFFER_OFFSET(indexAccessor.byteOffset));
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindVertexArray(0);


    }
  }
}
