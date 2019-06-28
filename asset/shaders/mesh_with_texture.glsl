#stage vertex
#include "version"

layout(location=0) in vec3 vs_Position;
layout(location=1) in vec3 vs_Normal;
layout(location=2) in vec2 vs_Texcoord;

out vec3 fs_Position;
out vec3 fs_Normal;
out vec2 fs_Texcoord;

uniform mat4 g_LocalTransform, g_WorldTransform;

void main()
{
    fs_Position = (g_LocalTransform*vec4(vs_Position, 1.)).xyz;
    gl_Position = g_WorldTransform*vec4(fs_Position, 1.);
    fs_Normal = mat3(transpose(inverse(g_LocalTransform)))*vs_Normal;
    fs_Texcoord = vs_Texcoord;
}

#endstage

#stage fragment
#include "version"

in vec3 fs_Position;
in vec3 fs_Normal;
in vec2 fs_Texcoord;

out vec4 out_Color;

uniform vec3 g_Eye, g_PointLightPos;

layout(binding=0) uniform sampler2D g_DiffuseMap;

void main()
{
    vec3 N = normalize(fs_Normal);
    vec3 L = normalize(g_PointLightPos-fs_Position);

    vec3 diffuseAlbedo = texture(g_DiffuseMap, fs_Texcoord).rgb;

    float cosNL = clamp(dot(N, L), 0., 1.);

	vec3 lighting = vec3(0., 0., 0.);

    lighting += cosNL * diffuseAlbedo;
    lighting += vec3(.5,.5,.5) * diffuseAlbedo;
    out_Color = vec4(diffuseAlbedo, 1);
}

#endstage
