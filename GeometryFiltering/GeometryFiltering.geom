#version 420 core

$GLMatrices

uniform float cylinderRadius;
uniform float cylinderHeight;
uniform vec3 cylinderAxis;
uniform vec3 cylinderPos;

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

in vec4 color[3];
out vec4 fragColor;

void main() {
	vec3 bottomFaceCenter = (viewMatrix * vec4(cylinderPos.x, cylinderPos.y, cylinderPos.z, 1.0)).xyz;
	vec3 topFaceCenter = bottomFaceCenter + ((viewMatrix * vec4(cylinderAxis.x, cylinderAxis.y, cylinderAxis.z, 0.0)).xyz * cylinderHeight);
	
	int verticesInsideCylinder = 0;
	for (int i = 0; i < gl_in.length(); ++i) {
		vec3 vertexPosWorld = gl_in[i].gl_Position.xyz;
		
		bool betweenCylinderFaces = (dot(vertexPosWorld - bottomFaceCenter, topFaceCenter - bottomFaceCenter) >= 0.0)
		                         && (dot(vertexPosWorld - topFaceCenter, bottomFaceCenter - topFaceCenter) >= 0.0);
		bool insideCylinderSurface = length(cross(vertexPosWorld - bottomFaceCenter, topFaceCenter - bottomFaceCenter)) 
		                           / length(topFaceCenter - bottomFaceCenter) <= cylinderRadius;

		if (betweenCylinderFaces && insideCylinderSurface) {
			++verticesInsideCylinder;
		}
	}

	for (int i = 0; i < gl_in.length(); ++i) {
		gl_Position = projMatrix * gl_in[i].gl_Position;
		fragColor = color[i];
		
		if (verticesInsideCylinder == 0) {
			EmitVertex();
		} else if (verticesInsideCylinder == 1) {
			fragColor = vec4(1.0, 0.0, 0.0, 1.0);
			EmitVertex();
		} else if (verticesInsideCylinder == 2) {
			fragColor = vec4(0.0, 1.0, 0.0, 1.0);
			EmitVertex();
		}
		
	}
	EndPrimitive();
	
}