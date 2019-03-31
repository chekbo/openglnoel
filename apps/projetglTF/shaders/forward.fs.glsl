#version 330 core

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;
uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;
uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;
uniform vec3 uKd;
uniform sampler2D uKdSampler;

out vec3 fFragColor;

void main() {
     
	fFragColor = vec3(0.0, 1.0, 0.0);
}
