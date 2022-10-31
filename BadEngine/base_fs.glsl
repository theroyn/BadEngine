#version 400

out vec4 frag_colour;

uniform vec3 eye_pos;
uniform vec3 object_color;
vec3 licht=vec3(1.2, 3.0, 2.0);

in vec3 frag_view_normal;
in vec4 frag_view_pos;
in vec3 frag_normal;
in vec3 frag_pos;
void main()
{
    // ambience
    vec3 light_color=vec3(1., 1., 1.);
    vec3 normal=normalize(frag_normal);

    float ambience_strength=0.1;
    vec3 ambience=ambience_strength * light_color;

    // diffuse
    vec3 to_light=normalize(licht - frag_pos);
    float diffuse_value=max(0., dot(to_light, normal));
    vec3 diffuse=light_color * diffuse_value;

    // specular
    vec3 reflect_light_to_eye=reflect(-to_light, normal);
    float specular_param=.5;
    float specular_value=pow(max(0., dot(reflect_light_to_eye, normalize(eye_pos - frag_pos.xyz))), 32);
    vec3 specular=light_color * specular_value * specular_param;
    vec3 res_col=(ambience + diffuse + specular) * object_color;

  //frag_colour = frag_view_pos;
  //frag_colour = vec4(0., 0., .2, 1.);
  //frag_colour = vec4(frag_view_normal, 1.);
    frag_colour=vec4(res_col, 1.);
}