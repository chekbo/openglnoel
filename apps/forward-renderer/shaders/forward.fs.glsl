#version 330 core

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

out vec3 fFragColor;

void main() {
  fFragColor = vViewSpaceNormal;
};
