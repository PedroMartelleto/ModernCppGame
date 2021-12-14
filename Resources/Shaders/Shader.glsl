#if (GLSL_VERSION >= 150)
	#define DeclareFragOutput(locationNumber, type) out type outputLocation##locationNumber
	#define SetFragOutput(locationNumber, val) outputLocation##locationNumber = val
	
	#define texture2D(tex, coord) texture(tex, coord)
	
	#if defined(VS_BUILD)
		#define varying out
		#define attribute in
	#elif defined(FS_BUILD)
		#define varying in
	#endif
#else
	#define DeclareFragOutput(locationNumber)
	#define SetFragOutput(locationNumber, val) gl_FragData[locationNumber] = val
#endif

varying vec4 v_Color;
varying vec2 v_TexCoord;
varying float v_TexIndex;

#if defined(VS_BUILD)

attribute vec3 a_Position;
attribute vec4 a_Color;
attribute vec2 a_TexCoord;
attribute float a_TexIndex;

uniform mat4 u_ViewProj;
uniform mat4 u_Transform;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	gl_Position = u_Transform * (u_ViewProj * vec4(a_Position, 1.0));
}

#elif defined(FS_BUILD)

uniform sampler2D u_Textures[32];

DeclareFragOutput(0, vec4);
void main()
{
	int index = int(v_TexIndex);
	vec4 texColor = clamp(texture2D(u_Textures[index], v_TexCoord) + 1/1000.0, 0.0, 1.0);

	if (texColor.a < 0.5) { discard; }

	SetFragOutput(0, texColor * v_Color);
}

#endif