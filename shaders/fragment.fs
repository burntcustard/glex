#version 130

in vec4 frag_color;

// Bad things happen if this isn't defined (again? As well as in translate.vs?) here. I don't know why :(
uniform vec4 cube_color;

out vec4 out_color;

void main() {
  out_color = vec4(frag_color);
}