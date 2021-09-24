#include "helper.h"
#define GLM_FORCE_RADIANS



#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"




static GLFWwindow* win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;

float *vertexBuffer;
int *indexBuffer;

unsigned int VAO;
float heightFactor = 10.0;
float speed = 0.0;
float realspeed = 0.0;

glm::vec3 gaze = glm::vec3(0, 0, 1);
glm::vec3 up = glm::vec3(0, 1, 0);
glm::vec3 cam_pos;

static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


void controls(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS){
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_O)
			heightFactor += 0.5;
		else if (key == GLFW_KEY_L)
			heightFactor -= 0.5;
		else if (key == GLFW_KEY_U)
			speed += 0.05;
		else if (key == GLFW_KEY_J)
			speed -= 0.05;
		else if (key == GLFW_KEY_D){
			glm::mat4 rotationMat(1); // Creates a identity matrix
			rotationMat = glm::rotate(rotationMat, -0.05f, up);
			gaze  = glm::vec3(rotationMat * glm::vec4(gaze, 1.0));
		}
		else if (key == GLFW_KEY_A){
			glm::mat4 rotationMat(1); // Creates a identity matrix
			rotationMat = glm::rotate(rotationMat, 0.05f, up);
			gaze = glm::vec3(rotationMat * glm::vec4(gaze, 1.0));
		}
		else if (key == GLFW_KEY_W){
			glm::mat4 rotationMat(1); // Creates a identity matrix
			rotationMat = glm::rotate(rotationMat, -0.05f, glm::normalize(glm::cross(up, gaze)));
			gaze = glm::vec3(rotationMat * glm::vec4(gaze, 1.0));
		}
		else if (key == GLFW_KEY_S){
			glm::mat4 rotationMat(1); // Creates a identity matrix
			rotationMat = glm::rotate(rotationMat, 0.05f, glm::normalize(glm::cross(up, gaze)));
			gaze = glm::vec3(rotationMat * glm::vec4(gaze, 1.0));
		}
		else if (key == GLFW_KEY_F){
			const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

			int window_width = mode->width;
			int window_height = mode->height;

			glfwSetWindowMonitor(win,
				glfwGetPrimaryMonitor(),
				0,
				0,
				window_width,
				window_height,
				GLFW_DONT_CARE
				);
		}



	}

}






void gen_indices(int w, int h, int* a){
	int k = 0;

	int m = 0, n = 1, x = w + 1, y = w + 2;
	for (int i = 0; i < h; i++){

		for (int j = 0; j < w; j++){
			a[k++] = n;
			a[k++] = m;
			a[k++] = y;

			a[k++] = x;
			a[k++] = y;
			a[k++] = m;

			m++, n++, x++, y++;
		}
		m++, n++, x++, y++;
	}
}

void gen_vertices(int w, int h, float* a){
	int k = 0;
	for (int i = 0; i < h + 1; i++){
		for (int j = 0; j < w + 1; j++){
			a[k++] = (float)j ;
			a[k++] =  0.0;
			a[k++] = (float)i;
		}
	}
}

void setCamera() {
	
	realspeed += speed;
	glEnable(GL_DEPTH_TEST); // Depth Testing
	// Draw stuff
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);


	int hf = glGetUniformLocation(idProgramShader, "heightFactor");
	glUseProgram(idProgramShader);
	glUniform1f(hf, heightFactor);



	

	cam_pos = glm::vec3(widthTexture / 2, widthTexture / 10, widthTexture / -4) + gaze*realspeed;

	int cp = glGetUniformLocation(idProgramShader, "cameraPosition");
	glUseProgram(idProgramShader);
	glUniform3fv(cp,1, glm::value_ptr(cam_pos));

	// Projection matrix
	glm::mat4 Projection = glm::perspective( 45.0f, 1.0f, 0.1f, 1000.0f);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(cam_pos.x, cam_pos.y , cam_pos.z), // camera pos
		glm::vec3(cam_pos.x + gaze.x, cam_pos.y + gaze.y, cam_pos.z + gaze.z), // gaze
		glm::vec3(up.x, up.y, up.z)  // up
		);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model;
	glm::mat4 mv =  View * Model;
	
	int MVPLocation = glGetUniformLocation(idProgramShader, "MVP");
	glUseProgram(idProgramShader);
	glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &mvp[0][0]);
	

	int MVLocation = glGetUniformLocation(idProgramShader, "MV");
	glUseProgram(idProgramShader);
	glUniformMatrix4fv(MVLocation, 1, GL_FALSE, &mv[0][0]);


	int widthLoc = glGetUniformLocation(idProgramShader, "widthTexture");
	glUseProgram(idProgramShader);
	glUniform1i(widthLoc, widthTexture);

	int heightLoc= glGetUniformLocation(idProgramShader, "heightTexture");
	glUseProgram(idProgramShader);
	glUniform1i(heightLoc, heightTexture);



}

void draw_map(){
	static bool firstTime = true;

	static int nVertices;
	static int nTriangles;

	
	if (firstTime){
		firstTime = false;



		nVertices = (widthTexture + 1) * (heightTexture + 1);
		nTriangles = 2 * widthTexture*heightTexture;

		vertexBuffer = new float[nVertices * 3];
		indexBuffer = new int[nTriangles * 3];


		gen_vertices(widthTexture, heightTexture, vertexBuffer);
		gen_indices(widthTexture, heightTexture, indexBuffer);


		
		glGenVertexArrays(1, &VAO);


		glBindVertexArray(VAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, nVertices * 3 * sizeof(float), vertexBuffer, GL_STATIC_DRAW);


		unsigned int EBO;
		glGenBuffers(1, &EBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTriangles * 3 * sizeof(int), indexBuffer, GL_STATIC_DRAW);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}



	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, nTriangles * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}





int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Please provide only a texture image\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  win = glfwCreateWindow(600, 600, "CENG477 - HW4", NULL, NULL);
  
  if (!win) {
      glfwTerminate();
      exit(-1);
  }
  glfwMakeContextCurrent(win);
  glfwSetKeyCallback(win, controls);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

      glfwTerminate();
      exit(-1);
  }

  initShaders();

  

  glUseProgram(idProgramShader);
  initTexture(argv[1], &widthTexture, &heightTexture);
  


  while(!glfwWindowShouldClose(win)) {

	  // Scale to window size
	  GLint windowWidth, windowHeight;
	  glfwGetWindowSize(win, &windowWidth, &windowHeight);
	  glViewport(0, 0, windowWidth, windowHeight);



	  setCamera();
	  draw_map();


    glfwSwapBuffers(win);
    glfwPollEvents();

  }


  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}
