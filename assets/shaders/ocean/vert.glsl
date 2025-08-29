#version 430 core

layout (location = 0) in vec3 vertex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out VS_OUT  {
    vec3 position_world_space;
    vec3 normal;
} vs_out;

void main() {
    vec3 normal;
    vec4 position_world_space;

    {
        position_world_space = model * vec4(vertex, 1.0);
        float b = position_world_space.x + time;
        position_world_space.y = sin(b);
        normal = normalize(
            vec3(
                -cos(b),
                1,
                0
            )
        );
    }

    {
        vs_out.position_world_space = position_world_space.xyz;
        vs_out.normal = normal;
    }
    
    gl_Position = projection  * view * position_world_space;
}