#version 420 core

in vec3 position;
in vec2 texCoord;

out VS_OUT {
	vec2 textureCoord;
} vs_out;

void main()
{
	// Use right half of word for x and left half for z
	int x = gl_InstanceID & 63;
	int z = gl_InstanceID >> 6;

	vec3 newPosition = position - vec3(float(x) - 31.5, 0.0, float(z) - 31.5);

	gl_Position = vec4(newPosition, 1.0);
	//vs_out.textureCoord = texCoord;

	vs_out.textureCoord.x = (texCoord.x - x) / 64.0;
	vs_out.textureCoord.y = (texCoord.y + z) / 64.0;
}
