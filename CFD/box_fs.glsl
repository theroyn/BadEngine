#version 400

out vec4 frag_colour;

void main()
{
  //frag_colour = frag_view_pos;
  frag_colour = vec4(0., 0.5, .2, 1.);
  //frag_colour = vec4(frag_view_normal, 1.);
  //frag_colour = vec4(frag_normal, 1.);
}