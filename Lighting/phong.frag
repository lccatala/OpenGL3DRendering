#version 420

$Lights
$Material

in vec4 color;
in vec3 fragN;
in vec3 fragPosition;

out vec4 fragColor;

vec4 iluminacion(vec3 pos, vec3 N, vec3 V) {
    // Componente emisiva del material
    vec4 color = emissive;
    for (int i = 0; i < lights.length(); i++) {
        if (lights[i].enabled == 0)
            continue;

        // If directional -> positionEye. Else -> positionEye - pos
        vec3 L = normalize(lights[i].positionEye.xyz + pos * (lights[i].directional - 1));
        
        // Multiplicador de la componente difusa
        float diffuseMult = max(dot(N, L), 0.0);
        float specularMult = 0.0;
        if (diffuseMult > 0.0) {
            // Multiplicador de la componente especular
            vec3 R = reflect(-L, N);
            specularMult = max(0.0, dot(R, V));
            specularMult = pow(specularMult, shininess);
        }
        
        float spot = 1.0;
        if (lights[i].spotCutoff < 180 && lights[i].directional == 0) {
            spot = max(dot(-L, lights[i].spotDirectionEye), 0.0);

            bool inside = spot >= lights[i].spotCosCutoff;
            if (inside) {
                spot = pow(spot, lights[i].spotExponent);
            } else {
                spot = 0.0;
            }
            
        }

        float d = distance(pos, lights[i].positionWorld.xyz);
        float attenuation = 1.0 / max(1, 
            lights[i].attenuation.x + lights[i].attenuation.y * d + lights[i].attenuation.z * d * d);
        if (lights[i].directional == 0)
            attenuation = 1.0; // TODO optimize this

        color += spot * attenuation 
               * (lights[i].ambient  * ambient 
               +  lights[i].diffuse  * diffuse  * diffuseMult 
               +  lights[i].specular * specular * specularMult);
    }
    
    return color;
}

void main() {
  vec3 eN = normalize(fragN);
  vec3 V = normalize(-fragPosition.xyz);
  fragColor = iluminacion(fragPosition, eN, V);
}
