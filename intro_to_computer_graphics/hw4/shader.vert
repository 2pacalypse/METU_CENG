#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec3 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

vec3 calc_normal(int x, int z){

    vec3 res = vec3(0,0,0);

    int k = 0;
	vec3 triangles[18];
	
	//tri 1
	triangles[k++] = vec3(x, 0, z);
	triangles[k++] = vec3(x, 0, z - 1);
	triangles[k++] = vec3(x - 1, 0, z - 1);
	//tri 2
	triangles[k++] = vec3(x, 0, z);
	triangles[k++] = vec3(x - 1, 0, z -1);
	triangles[k++] = vec3(x - 1, 0, z);
	//tri 3
	triangles[k++] = vec3(x, 0, z);
	triangles[k++] = vec3(x - 1, 0, z);
	triangles[k++] = vec3(x, 0, z + 1);
	//tri 4
	triangles[k++] = vec3(x, 0, z);
	triangles[k++] = vec3(x, 0, z+1);
	triangles[k++] = vec3(x + 1, 0, z+1);
	//tri 5
	triangles[k++] = vec3(x, 0, z);
	triangles[k++] = vec3(x+1, 0, z+1);
	triangles[k++] = vec3(x + 1, 0, z);
	//tri 6
	triangles[k++] = vec3(x, 0, z);
	triangles[k++] = vec3(x+1, 0, z);
	triangles[k++] = vec3(x, 0, z -1);

	for(int i = 0; i < 18; i++){
	    vec3 old = triangles[i];

		if(old.x >= 0 && old.z >= 0 && old.x <= widthTexture && old.z <= heightTexture ){
			float x = 1 - old.x / widthTexture;
	        float y = 1 - old.z / heightTexture;
			textureCoordinate = vec2(x,y);

			vec4 pColor = texture2D(rgbTexture, textureCoordinate);
			float h = 0.2126*pColor.x + 0.7152*pColor.y + 0.0722*pColor.z;
			h *= heightFactor;

		    triangles[i] = vec3(old.x, h, old.z);
		}else{
		    triangles[i] = vec3(-1, -1, -1);
		}
	}

	for(int i = 0; i < 6; i++){
	    vec3 a = triangles[3*i];
		vec3 b = triangles[3*i + 1];
		vec3 c = triangles[3*i + 2];

		if(a == vec3(-1,-1,-1) || b == vec3(-1,-1,-1) || c == vec3(-1,-1,-1)){
		       
		}else{

		    vec3 v1 = b - a;
			vec3 v2 = c - a;
			vec3 n = cross(v1, v2);
			res += n;
		}
	}
	return res;
}

void main()
{


    // get texture value, compute height

	float x = 1 - position.x / widthTexture;
	float y = 1 - position.z / heightTexture;

	textureCoordinate = vec2(x,y);

	vec4 pColor = texture2D(rgbTexture, textureCoordinate);
	float h = 0.2126*pColor.x + 0.7152*pColor.y + 0.0722*pColor.z;
	h *= heightFactor;

	vec4 vertexPos = vec4(position.x, h, position.z, 1.0);


	mat3 normalMatrix = mat3(transpose(inverse(MV)));
	
    // compute normal vector using also the heights of neighbor vertices
	vertexNormal = calc_normal(int(position.x), int(position.z));
	vertexNormal = normalMatrix * vertexNormal;
	


    // compute toLight vector vertex coordinate in VCS

   
	ToLightVector =  vec3(MV*vertexPos);
	ToCameraVector = vec3(MV*vertexPos);

   
   // set gl_Position variable correctly to give the transformed vertex position
   gl_Position =  MVP * vertexPos;
    
}
