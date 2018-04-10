#version 120

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;

attribute vec4 gl_Vertex;

void main() {
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}