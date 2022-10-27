#version 400

layout(location=0) in vec3 pos;
layout(location=1) in vec3 in_normal;

// View oriented
out vec3 frag_view_normal;
out vec4 frag_view_pos;

out vec3 frag_normal;
out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat3 normal_matrix=transpose(inverse(mat3(view * model)));

    vec4 view_pos=view * model * vec4(pos, 1.0);

    frag_view_normal=normal_matrix * in_normal;
    frag_view_pos=view_pos;

    frag_normal=mat3(transpose(inverse(model))) * in_normal;
    frag_pos=(model * vec4(pos, 1.0)).xyz;
    gl_Position=projection * view_pos;
}