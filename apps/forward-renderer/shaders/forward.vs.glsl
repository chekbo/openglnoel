#version 330 core

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexture;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

out vec3 vViewSpacePosition;
out vec3 vViewSpaceNormal;
out vec2 vTexCoords;

void main() {
  vViewSpacePosition = vec3(uModelViewMatrix * vec4(aVertexPosition,1));
  vViewSpaceNormal = vec3(uNormalMatrix * vec4(aNormal,0));
  vTexCoords = aTexture;
  gl_Position = uModelViewProjMatrix * vec4(aVertexPosition,1);
};