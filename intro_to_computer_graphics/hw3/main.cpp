#include <iostream>
#include "parser.h"
#include <cmath>
#include <algorithm>
#include <stack>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//////-------- Global Variables -------/////////



// Sample usage for reading an XML scene file
parser::Scene scene;
GLfloat *vertices;
GLfloat *normals;
parser::Vec3f og_gaze;
static GLFWwindow* win = NULL;


static void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

float alpha;

void pressU(){
	parser::Vec3f left = scene.camera.gaze.crossProduct(scene.camera.up);


	GLfloat mat[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(-0.5, left.x, left.y, left.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	glPopMatrix();

	float oldx = scene.camera.gaze.x ;
	float oldy = scene.camera.gaze.y ;
	float oldz = scene.camera.gaze.z ;




	scene.camera.gaze.x =  + mat[0] * oldx + mat[1] * oldy + mat[2] * oldz + mat[3] * 1;
	scene.camera.gaze.y = + mat[4] * oldx + mat[5] * oldy + mat[6] * oldz + mat[7] * 1;
	scene.camera.gaze.z =  + mat[8] * oldx + mat[9] * oldy + mat[10] * oldz + mat[11] * 1;



}

void pressJ(){
	parser::Vec3f left = scene.camera.gaze.crossProduct(scene.camera.up);

	GLfloat mat[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(+0.5, left.x, left.y, left.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	glPopMatrix();

	float oldx = scene.camera.gaze.x ;
	float oldy = scene.camera.gaze.y ;
	float oldz = scene.camera.gaze.z ;

	scene.camera.gaze.x =  + mat[0] * oldx + mat[1] * oldy + mat[2] * oldz + mat[3] * 1;
	scene.camera.gaze.y =  + mat[4] * oldx + mat[5] * oldy + mat[6] * oldz + mat[7] * 1;
	scene.camera.gaze.z =  + mat[8] * oldx + mat[9] * oldy + mat[10] * oldz + mat[11] * 1;



}

void pressW(){
	/**/
	//parser::Vec3f normalized = (scene.camera.gaze  + scene.camera.position);
	//normalized = normalized / sqrt(normalized.distance(parser::Vec3f{0,0,0}));
	//normalized = normalized*(0.05);


	scene.camera.position = scene.camera.position + scene.camera.gaze*0.5;

	/*scene.camera.position.x = oldx +   mat[0] * oldx + mat[1] * oldy + mat[2] * oldz + mat[3] * 1;
	scene.camera.position.y = oldy +  mat[4] * oldx + mat[5] * oldy + mat[6] * oldz + mat[7] * 1;
	scene.camera.position.z =  oldz + mat[8] * oldx + mat[9] * oldy + mat[10] * oldz + mat[11] * 1;
	*/


}

void pressS(){
	/**/

	scene.camera.position = scene.camera.position - scene.camera.gaze*0.5;


}

float beta;



void pressA(){

	//scene.camera.near_plane.y -= scene.camera.near_distance*tan(0.5*3.14 / 180);
	//scene.camera.near_plane.x -= scene.camera.near_distance*tan(0.5*3.14 / 180);
	GLfloat mat[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(-0.5, scene.camera.up.x, scene.camera.up.y, scene.camera.up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	glPopMatrix();

	float oldx = scene.camera.gaze.x ;
	float oldy = scene.camera.gaze.y ;
	float oldz = scene.camera.gaze.z ;

	scene.camera.gaze.x = +  mat[0] * oldx + mat[1] * oldy + mat[2] * oldz + mat[3]*1;
	scene.camera.gaze.y = +  mat[4] * oldx + mat[5] * oldy + mat[6] * oldz + mat[7] * 1;
	scene.camera.gaze.z =  +  mat[8] * oldx + mat[9] * oldy + mat[10] * oldz + mat[11] * 1;


}


void pressD(){
	//scene.camera.near_plane.y -= scene.camera.near_distance*tan(0.5*3.14 / 180);
	//scene.camera.near_plane.x -= scene.camera.near_distance*tan(0.5*3.14 / 180);
	GLfloat mat[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(0.5, scene.camera.up.x, scene.camera.up.y, scene.camera.up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	glPopMatrix();

	float oldx = scene.camera.gaze.x ;
	float oldy = scene.camera.gaze.y ;
	float oldz = scene.camera.gaze.z ;

	scene.camera.gaze.x = +  mat[0] * oldx + mat[1] * oldy + mat[2] * oldz + mat[3]*1;
	scene.camera.gaze.y = +  mat[4] * oldx + mat[5] * oldy + mat[6] * oldz + mat[7] * 1;
	scene.camera.gaze.z =  +  mat[8] * oldx + mat[9] * oldy + mat[10] * oldz + mat[11] * 1;
}

void setModelTransforms(const parser::Mesh& m){
	std::stack <parser::Transformation> stack;
	for (const parser::Transformation& t : m.transformations){
		stack.push(t);
	}
	glMatrixMode(GL_MODELVIEW);

	while (!stack.empty()){
		const parser::Transformation& t = stack.top();
		if (t.transformation_type[0] == 'R'){
			const parser::Vec4f& v = scene.rotations[t.id - 1];
			glRotatef(v.x, v.y, v.z, v.w);
		}
		else if (t.transformation_type[0] == 'T'){
			const parser::Vec3f& v = scene.translations[t.id - 1];
			glTranslatef(v.x, v.y, v.z);
		}
		else if (t.transformation_type[0] == 'S'){
			const parser::Vec3f& v = scene.scalings[t.id - 1];
			glScalef(v.x, v.y, v.z);
		}
		stack.pop();
	}

}

void turnOnLights() {



	for (int i = 0; i < scene.point_lights.size(); i++) {

		GLfloat col[] = { scene.point_lights[i].intensity.x, scene.point_lights[i].intensity.y, scene.point_lights[i].intensity.z, 1.0f };
		GLfloat pos[] = { scene.point_lights[i].position.x, scene.point_lights[i].position.y, scene.point_lights[i].position.z, 1.0f };
		GLfloat amb[] = { scene.ambient_light.x, scene.ambient_light.y, scene.ambient_light.z, 1.0f };

		glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, amb);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, col);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, col);
	}
}



void setCamera(const parser::Mesh& m) {

	glViewport(0, 0, scene.camera.image_width, scene.camera.image_height);
	/* Set camera position */


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		scene.camera.position.x,
		scene.camera.position.y,
		scene.camera.position.z,
		scene.camera.gaze.x +scene.camera.position.x ,
		scene.camera.gaze.y  +scene.camera.position.y,
		scene.camera.gaze.z  +scene.camera.position.z,
		scene.camera.up.x ,
		scene.camera.up.y ,
		scene.camera.up.z );
	/* Set projection frustrum */
	turnOnLights();
	


	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	/*gluPerspective(
		atan((scene.camera.near_plane.w-scene.camera.near_plane.z)/(	scene.camera.near_distance))*180/3.14159265358979, 
		(double)scene.camera.image_width / scalingsene.camera.image_height,
		 scene.camera.near_distance, 
		 scene.camera.far_distance);
*/		 
	
	glFrustum(
		scene.camera.near_plane.x,
		scene.camera.near_plane.y,
		scene.camera.near_plane.z,
		scene.camera.near_plane.w,
		scene.camera.near_distance,
		scene.camera.far_distance
		);

}




void drawObject(const parser::Mesh& mesh){

	static bool firstTime = true;


	if (firstTime)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		firstTime = false;


		size_t nVertices = scene.vertex_data.size();
		vertices = new GLfloat[nVertices*3];
		normals = new GLfloat[nVertices * 3];
		int i = 0;
		int k = 0;
		for (const parser::Vec3f& v : scene.vertex_data){

			parser::Vec3f normal{ 0, 0, 0 };
			int nNormal = 0;
			for (const parser::Mesh& m : scene.meshes){
				for (const parser::Face& f : m.faces){
					if (((f.v0_id - 1 ) == (i)/3) || ((f.v1_id  - 1) == (i)/3 ) || ((f.v2_id - 1) == (i)/3 )){
						parser::Vec3f a = scene.vertex_data[f.v2_id - 1] - scene.vertex_data[f.v1_id - 1];
						parser::Vec3f b = scene.vertex_data[f.v0_id - 1] - scene.vertex_data[f.v1_id - 1];
						parser::Vec3f cp = a.crossProduct(b);
						normal = normal + (cp);
						nNormal++;
					}
				}
			}



			float len = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
			if (len != 0)
				normal = normal / (len);


			normals[k++] = normal.x ;
			normals[k++] = normal.y ;
			normals[k++] = normal.z ;



			vertices[i++] = v.x;
			vertices[i++] = v.y;
			vertices[i++] = v.z;

		}



		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glNormalPointer(GL_FLOAT, 0, normals);



	}

	size_t nFaces = mesh.faces.size();
	GLuint *indices = new GLuint[nFaces*3];


	int j = 0;
	for (const parser::Face& f : mesh.faces){
		indices[j++] = f.v0_id - 1;
		indices[j++] = f.v1_id - 1;
		indices[j++] = f.v2_id - 1;
	}



	// At every frame, vertex and color data is copied from vertexPos
	// and vertexCol to GPU memory and the elements indicated by
	// indices are drawn

	//INDICE SAYISINI GIR BURAYA 6 YERINE, 6 VERTEX CIZIYORSUN YANI


	const parser::Material material = scene.materials[mesh.material_id - 1];

	GLfloat ambcolor[4] = { material.ambient.x, material.ambient.y, material.ambient.z, 1.0};
	GLfloat diffcolor[4] = { material.diffuse.x, material.diffuse.y, material.diffuse.z, 1.0f };
	GLfloat specColor[4] = { material.specular.x, material.specular.y, material.specular.z, 1.0f };
	GLfloat specExp[1] = { material.phong_exponent };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambcolor);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffcolor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specColor);
	glMaterialfv(GL_FRONT, GL_SHININESS, specExp);



	glDrawElements(GL_TRIANGLES, nFaces*3, GL_UNSIGNED_INT, indices);
	delete[] indices;
}






void controls(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static bool toggleLight = true;
	static size_t nLights = scene.point_lights.size();
	if (action == GLFW_PRESS){
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_U)
			pressU();
		else if (key == GLFW_KEY_J)
			pressJ();
		else if (key == GLFW_KEY_W)
			pressW();
		else if (key == GLFW_KEY_S)
			pressS();
		else if (key == GLFW_KEY_A)
			pressA();
		else if (key == GLFW_KEY_D)
			pressD();
		else if (key == GLFW_KEY_0){
			toggleLight = toggleLight ? false : true;
			if (toggleLight){
				for (int i = 0; i < scene.point_lights.size(); i++) {
					scene.point_lights[i].status = true;
					glEnable(GL_LIGHT0 + i);
				}
			}

			else{
				for (int i = 0; i < scene.point_lights.size(); i++) {
					scene.point_lights[i].status = false;
					glDisable(GL_LIGHT0 + i);
				}
			}

		}
		else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9){
			if (key - GLFW_KEY_1 < nLights){
				scene.point_lights[key - GLFW_KEY_1].status = scene.point_lights[key - GLFW_KEY_1].status ? false : true;
				if (scene.point_lights[key - GLFW_KEY_1].status){
					glEnable(GL_LIGHT0 + key - GLFW_KEY_1);
				}
				else{
					glDisable(GL_LIGHT0 + key - GLFW_KEY_1);
				}
			}
		}

	}


}



/*void turnOffLights() {
	glDisable(GL_LIGHTING);
	for (int i = 0; i < nlights; i++)
		glDisable(GL_LIGHT0 + I);
}*/


GLFWwindow* initWindow(const int resX, const int resY)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow(resX, resY, "CENG477", NULL, NULL);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, controls);


	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	// Get info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

	glEnable(GL_DEPTH_TEST); // Depth Testing
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
//	glEnable(GL_NORMALIZE);
	//GLfloat  amb[4] = { scene.ambient_light.x, scene.ambient_light.y, scene.ambient_light.z ,1.0f};
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

	for (int i = 0; i < scene.point_lights.size(); i++) {
		glEnable(GL_LIGHT0 + i);
	}

	return window;
}





void display(GLFWwindow* window)
{
	while (!glfwWindowShouldClose(window))
	{

		// Scale to window size
		GLint windowWidth, windowHeight;
		//glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);

		// Draw stuff
		glClearColor(scene.background_color.x, scene.background_color.y, scene.background_color.z, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		for (const parser::Mesh& m : scene.meshes){
			if (m.mesh_type[0] == 'S'){
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

		
			setCamera(m);
			setModelTransforms(m);
			drawObject(m);
			
		}


		// Update Screen
		glfwSwapBuffers(window);

		// Check for any input, or window movement
		glfwPollEvents();
	}
}




int main(int argc, char* argv[]) {
    scene.loadFromXml(argv[1]);
    //og_pos = scene.camera.position;
    //scene.camera.gaze = scene.camera.gaze + scene.camera.position;
    //scene.camera.gaze = scene.camera.gaze / sqrt(scene.camera.gaze.distance(parser::Vec3f{0,0,0}));


	GLFWwindow* window = initWindow(scene.camera.image_width, scene.camera.image_height	);
	if (NULL != window)
	{
		display(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();



    return 0;

	}
