#version 330 core

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTexture;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

out vec3 vViewSpacePosition;
out vec3 vViewSpaceNormal;
out vec3 vTexCoords;
out vec3 vFragColor;

void main() {
  vViewSpacePosition = uModelViewMatrix * aPosition
  vViewSpaceNormal = uNormalMatrix * aNormal			
  vTexCoords = aTexCoords
  gl_Position = uModelViewProjMatrix * aPosition
};