
uniform sampler2D myTexture;

varying vec3 normalVec;
varying float vFog;

void main (void) {
	vec3 lightVector=vec3(1,1,0);
	vec4 fogColor=vec4(255.0,255.0,0.0,0.0);
	
	float factor=clamp(dot(normalVec,lightVector),0.65,1.0);
	vec4 col = texture2D(myTexture, vec2(gl_TexCoord[0]));
	gl_FragColor = mix(col*factor,fogColor,vFog);
}
