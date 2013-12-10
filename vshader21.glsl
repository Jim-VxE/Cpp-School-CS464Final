




in  vec2 vTexCoord;
out vec2 texCoord;

in   vec4 vPosition;
in   vec3 vNormal;

// output values that will be interpretated per-fragment
out  vec3 fN;
out  vec3 fE;
out  vec3 fL;

uniform vec4 LightPosition;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float texScale;

void main()
{
    vec3 pos = (view * model * vPosition).xyz;
    fN = (view*model*vec4(vNormal, 0.0)).xyz;
    fE = -pos;
    fL = (view * LightPosition).xyz - pos;
    
    if( LightPosition.w != 0.0 ) {
        fL = LightPosition.xyz - vPosition.xyz;
    }
    
    gl_Position = projection * view * model * vPosition;
    texCoord    = vTexCoord;
}
