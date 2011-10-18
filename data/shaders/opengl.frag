
uniform sampler2D myTexture;

varying vec3 normalVec;

void main (void) {
	vec3 lightVector=vec3(1,1,0);
	vec4 fogColor=vec4(0.227,0.204,0.0,1.0);
	
	float z = ((gl_FragCoord.z/gl_FragCoord.w)-1750.0)/350.0;
	
	float fogFact=clamp(z,0.0,1.0);
	
	float factor=clamp(dot(normalVec,lightVector),0.65,1.0);
	vec4 col = texture2D(myTexture, vec2(gl_TexCoord[0]));
	gl_FragColor = mix(col*factor,fogColor,fogFact);
}
