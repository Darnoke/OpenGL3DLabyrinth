#version 440

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in float crate;

out vec4 fs_color;

void main()
{
	if (crate > 0.5f) 
	{
		float sum = vs_position.x * 50 + vs_position.y * 50 + vs_position.z * 50;
		if (mod(sum, 2.0) <= 0.5f) fs_color = vec4(1.f, 1.f, 1.f, 1.f);
		else fs_color = vec4(0.f, 0.f, 0.f, 0.f);
	}
	else fs_color = vec4(vs_color, 1.f);
}