#version 330 core

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;
uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;
uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;
uniform vec3 uKd;
uniform sampler2D uKdSampler

out vec3 fFragColor;

void main() {
     float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
     vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;
     fFragColor = texture(uKdSampler,vTexCoords)*  uKd * (uDirectionalLightIntensity * max(0.0, dot(vViewSpaceNormal, uDirectionalLightDir)) + uPointLightIntensity * max(0.0, dot(vViewSpaceNormal, dirToPointLight)) / (distToPointLight * distToPointLight));
};
