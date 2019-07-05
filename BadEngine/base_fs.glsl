#version 400

out vec4 frag_colour;

in vec3 frag_view_normal;
in vec4 frag_view_pos;
in vec3 frag_normal;
void main()
{
  //frag_colour = frag_view_pos;
  //frag_colour = vec4(0., 0., .2, 1.);
  //frag_colour = vec4(frag_view_normal, 1.);
  frag_colour = vec4(frag_normal, 1.);
}