varying vec4 GBufferNormal;
varying vec4 GBufferVertex; 
varying vec3 GBufferColor;
varying vec2 GBufferTextureCoordinates;
uniform int useTexture;
uniform int useMeshColor;
uniform sampler2D texture;

void main()
{	

	gl_FragData[0] = GBufferVertex;
	gl_FragData[1] = vec4(GBufferNormal.xyz, float(gl_FrontFacing));
	if(useTexture == 1) gl_FragData[2] = texture2D(texture, GBufferTextureCoordinates);
	else if(useMeshColor == 1) gl_FragData[2] = vec4(GBufferColor, 1.0);
	else gl_FragData[2] = vec4(1.0, 1.0, 1.0, 0.0);

}