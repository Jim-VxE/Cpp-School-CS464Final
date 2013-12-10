
in  vec2 texCoord;
out vec4 fColor;

uniform sampler2D texture;


// per-fragment interpolated values from the vertex shader
in  vec3 fN;
in  vec3 fL;
in  vec3 fE;

uniform vec4 AmbientLight, DiffuseLight, SpecularLight;
uniform vec4 ka, kd, ks;
uniform float Shininess;

uniform float texScale;

void main()
{
    // Normalize the input lighting vectors
    vec3 N = normalize(fN);
    vec3 E = normalize(fE);
    vec3 L = normalize(fL);
    
    vec3 H = normalize( L + E );
    
    
    
    // Compute terms in the illumination equation
    vec4 ambient = AmbientLight * ka;
    
    vec4  diffuse = DiffuseLight * kd * texture2D( texture, texCoord * texScale ) * max( dot(L, N), 0.0 );///(0.2*pow(length((view * LightPosition).xyz - pos),2.0));
    
    vec4  specular = SpecularLight * ks * pow( max(dot(N, H), 0.0), Shininess );
    
    if ( dot(L, N) < 0.0 ) specular = vec4(0.0, 0.0, 0.0, 1.0);
    
    
    
 //   gl_FragColor = ambient + diffuse + specular;
  //  gl_FragColor.a = 1.0;

	fColor = ambient + diffuse + specular;
	fColor.a = 1.0;
}
