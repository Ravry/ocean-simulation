#version 430 core

out vec4 color;

in VS_OUT  {
    vec3 position_world_space;
    vec3 normal;
} fs_in;

float calc_lighting(vec3 normal) {
    const vec3 light_direction = vec3(0, -1, 0);
    float result = max(dot(normal, -light_direction), .2);
    return result;
}

void main() {
    float lighting = calc_lighting(fs_in.normal); 
    color = vec4(lighting * vec3(0, 0, 1), 1);
}