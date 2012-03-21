
uniform mat4 mWorldViewProj;
uniform mat4 mInvWorld;
uniform mat4 mTransWorld;
uniform float mMinFog;
uniform float mMaxFog;

varying vec3 normalVec;

void main(void)
{
	gl_Position = mWorldViewProj * gl_Vertex;
	
	vec4 normal = vec4(gl_Normal, 0.0);
	normal = mInvWorld * normal;
	normal = normalize(normal);
	
	normalVec[0]=normal[0];
	normalVec[1]=normal[1];
	normalVec[2]=normal[2];
	
	vec4 worldpos = gl_Vertex * mTransWorld;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	float fog_coord; // distance for fog calculation...
	// This function is generally faster and is guaranteed
	// to produce the same result on each run...
	// gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	//gl_Position = ftransform();
	vec4 thePos=gl_ModelViewProjectionMatrix*gl_Vertex;
	fog_coord = abs(thePos.z);
	fog_coord = clamp( fog_coord, mMinFog, mMaxFog);
}
