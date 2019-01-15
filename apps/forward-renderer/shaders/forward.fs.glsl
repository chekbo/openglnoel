#version 330 core

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec3 vTexCoords;
in vec3 vFragColor;

out vec3 fFragColor;

void main() {
  fFragColor = vViewSpaceNormal;
};
