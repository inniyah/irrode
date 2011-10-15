
uniform sampler2D myTexture;

varying vec3 normalVec;

void main (void) {
	vec3 lightVector=vec3(1,1,0);
	
	float factor=clamp(dot(normalVec,lightVector),0.75,1.0);
	vec4 col = texture2D(myTexture, vec2(gl_TexCoord[0]));
	gl_FragColor = col*factor;
}
