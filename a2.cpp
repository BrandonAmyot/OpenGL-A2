// Brandon Amyot 26990940
// modified from http://learnopengl.com

#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include <glm.hpp>			// include GL Math
#include "gtc\matrix_transform.hpp"
#include "gtc\type_ptr.hpp"
#include <type_vec3.hpp>

using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;
const float ASPECT = float(WIDTH) / HEIGHT;

// ???
glm::vec3 triangle_scale;
glm::vec3 camera_translation = glm::vec3(0.0f, 0.0f, -1.0f);

// vector curves
vector<glm::vec3*>* profCurve = new vector<glm::vec3*>;
vector<glm::vec3*>* trajCurve = new vector<glm::vec3*>;
vector<glm::vec3*>* projPoints = new vector<glm::vec3*>;
vector<glm::vec3*>* trajPoints = new vector<glm::vec3*>;


// to determine first or second spline
bool secondCurve = false;

// declare VAOs/VBOs
GLuint VAO, VBO, VAOpoints, VBOpoints;

//initialize variables for draw function
GLfloat* vertices;
int objectType;
int profileCurveNum;
int trajCurveNum;
string line;
float x, y, z;
int arraySize, index, indicesSize, indicesIndex;
int *indices;

// ???
float rotation = 9.0;

// Declare the matrices
glm::mat4 model_matrix;
glm::mat4 view_matrix;
glm::mat4 proj_matrix;

const float TRIANGLE_MOVEMENT_STEP = 0.5f;
const float CAMERA_PAN_STEP = 0.2f;

GLfloat radius = 10.0f;
GLfloat camX = sin(glfwGetTime()) * radius;
GLfloat camZ = cos(glfwGetTime()) * radius;
glm::mat4 view;

float xValue = 0.0;
float zValue = 0.0;
float cameraZoom = 3.0f;
double xpos;
double ypos;

// Forward declaration
void updatePoints();
void renderSpline();
void draw();


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		zValue -= 10;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		zValue += 10;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		xValue += 10;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		xValue -= 10;

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		if (objectType == 0 && secondCurve == true) {
			//renderspline2();
			//output to file
		}
		else if (objectType == 0) {
			secondCurve = true;
			//renderspline1();
		}
		else {
			//renderspline();
			//output to file
		}
	}

	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {

	}


	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		glPointSize(10);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

	cameraZoom += (float)yoffset * 0.05; // 0.05 to help slow down the zooming
	cout << "scrolling" << endl;
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int w = height * ASPECT;
	int left = (width - w) / 2;

	glViewport(left, 0, w, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &xpos, &ypos);
		
		updatePoints();
	}
}

void updatePoints() {
	if (secondCurve) {
		trajPoints->push_back(new glm::vec3(xpos, ypos, 0.0f));

		arraySize = trajPoints->size() * 3;
		vertices = new GLfloat[arraySize];
		index = 0;

		for (int p = 0; p < trajPoints->size(); p++) {
			vertices[index] = (trajPoints->at(p)->x) * 2 / WIDTH - 1;
			vertices[index + 1] = (trajPoints->at(p)->y) * 2 / HEIGHT;
			vertices[index + 2] = trajPoints->at(p)->z;
			index += 3;
		}
		glBindVertexArray(VAOpoints);
		glBindBuffer(GL_ARRAY_BUFFER, VBOpoints);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * arraySize, vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}
	else {
		projPoints->push_back(new glm::vec3(xpos, 0.0f, ypos));

		arraySize = projPoints->size() * 3;
		vertices = new GLfloat[arraySize];
		index = 0;

		for (int p = 0; p < projPoints->size(); p++) {
			vertices[index] = (projPoints->at(p)->x) * 2 / WIDTH - 1;
			cout << vertices[index] << endl;
			vertices[index + 1] = projPoints->at(p)->y;
			cout << vertices[index+1] << endl;
			vertices[index + 2] = 1 - ((projPoints->at(p)->z) * 2 / HEIGHT);
			cout << vertices[index+2] << endl;
			index += 3;
		}
		cout << endl;

		glBindVertexArray(VAOpoints);
		glBindBuffer(GL_ARRAY_BUFFER, VBOpoints);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * arraySize, vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}
}

void draw() {
	// Get input from file
		ifstream inputFile;
		inputFile.open("input.txt");

	//// initialize vertex array and counters
	//GLfloat* vertices;
	//int objectType;
	//int profileCurveNum;
	//int trajCurveNum;
	//string line;
	//float x, y, z;
	//int size, index, indicesSize, indicesIndex;
	//int *indices;

	// get object type (sweep/ rotation)
	getline(inputFile, line);
	objectType = stoi(line);

	if (objectType == 0) { // translational sweep

		// profile curve
		getline(inputFile, line);
		profileCurveNum = stoi(line);
		for (int i = 0; i < profileCurveNum; i++) {
			getline(inputFile, line);
			stringstream lineStream(line);

			// get the x, y, z and store them
			lineStream >> x >> y >> z;
			profCurve->push_back(new glm::vec3(x, y, z));
		}

		// trajectory curve
		getline(inputFile, line);
		trajCurveNum = stoi(line);
		for (int i = 0; i < trajCurveNum; i++) {
			getline(inputFile, line);
			stringstream lineStream(line);

			// get the x, y, z and store them
			lineStream >> x >> y >> z;
			trajCurve->push_back(new glm::vec3(x, y, z));
		}

		// initialize array sizes and counters 
		index = 0, indicesIndex = 0;
		arraySize = profCurve->size() * trajCurve->size() * 6;
		vertices = new GLfloat[arraySize];
		indicesSize = ((profCurve->size() - 1) * (trajCurve->size() - 1) * 6);
		indices = new int[indicesSize];

		// loop through curves to get vertices
		for (int p = 0; p < profCurve->size(); p++) {
			glm::vec3* pVec = profCurve->at(p);

			for (int t = 0; t < trajCurve->size(); t++) {
				glm::vec3* tVec = trajCurve->at(t);

				// set height variable to adjust the colour
				float height = (float(p) / float(profCurve->size()));

				vertices[index] = pVec->x + tVec->x;
				vertices[index + 1] = pVec->y + tVec->y;
				vertices[index + 2] = pVec->z + tVec->z;
				vertices[index + 3] = height;
				vertices[index + 4] = height + 0.5;
				vertices[index + 5] = 0.5;
				index += 6;

				// set up the indices
				if (t > 0 && p > 0) {
					indices[indicesIndex] = index / 6 - 1;
					indices[indicesIndex + 1] = index / 6 - 1 - 1;
					indices[indicesIndex + 2] = index / 6 - trajCurve->size() - 2;

					indices[indicesIndex + 3] = index / 6 - 1;
					indices[indicesIndex + 4] = index / 6 - trajCurve->size() - 1;
					indices[indicesIndex + 5] = index / 6 - trajCurve->size() - 2;
					indicesIndex += 6;
				}
			}
		}
	}
	else if (objectType != 0) { // rotational sweep
		int numOfSpans = 0;

		// get number of spans
		getline(inputFile, line);
		numOfSpans = stoi(line);

		// get curve profile
		getline(inputFile, line);
		profileCurveNum = stoi(line);
		for (int i = 0; i < profileCurveNum; i++) {
			getline(inputFile, line);
			stringstream lineStream(line);

			// get the x, y, z and store them
			lineStream >> x >> y >> z;
			profCurve->push_back(new glm::vec3(x, y, z));
		}

		// initialize array sizes and counters
		index = 0, indicesIndex = 0;
		arraySize = profCurve->size() * numOfSpans * 6;
		vertices = new GLfloat[arraySize];
		indicesSize = ((profCurve->size() - 1) * numOfSpans * 6);
		indices = new int[indicesSize];


		glm::mat4x4 rotation = glm::mat4(1.0f);

		// loop through curves to get vertices
		for (int s = 0; s < numOfSpans; s++) {

			for (int t = 0; t < profileCurveNum; t++) {

				glm::vec4 newVec = glm::vec4(*profCurve->at(t), 1.0);
				glm::vec4 rotationVec = newVec * glm::rotate(rotation, glm::radians(360.0f / (float)numOfSpans)*s, glm::vec3(0, 0, 1));

				// set height variable to adjust the colour
				float height = (float(t) / float(profCurve->size()));

				vertices[index] = rotationVec.x;
				vertices[index + 1] = rotationVec.y;
				vertices[index + 2] = rotationVec.z;
				vertices[index + 3] = height;
				vertices[index + 4] = height + 0.5;
				vertices[index + 5] = 0.5;
				index += 6;

				// set up the indices
				if (t > 0 && s > 0) {
					indices[indicesIndex] = index / 6 - 1;
					indices[indicesIndex + 1] = index / 6 - 2;
					indices[indicesIndex + 2] = index / 6 - profCurve->size() - 2;

					indices[indicesIndex + 3] = index / 6 - 1;
					indices[indicesIndex + 4] = index / 6 - profCurve->size() - 1;
					indices[indicesIndex + 5] = index / 6 - profCurve->size() - 2;
					indicesIndex += 6;
				}
				else if (t > 0) {
					indices[indicesIndex] = index / 6 - 1;
					indices[indicesIndex + 1] = index / 6 - 1 - 1;
					indices[indicesIndex + 2] = arraySize / 6 - profCurve->size() + index / 6 - 2;

					indices[indicesIndex + 3] = index / 6 - 1;
					indices[indicesIndex + 4] = arraySize / 6 - profCurve->size() + index / 6 - 1;
					indices[indicesIndex + 5] = arraySize / 6 - profCurve->size() + index / 6 - 2;
					indicesIndex += 6;
				}
			}
		}
	}
}

// draw the splines
void renderSpline() {

}
// Function to call at the beginning to check whether we will be rendering a translational or rotational sweep.
void askForInput() {
	string input;
	cout << "Enter which type of object you would like to render \nT: translational sweep \nR: rotational sweep" << endl;
	cin >> input;
	if (input[0] == 't' || input[0] == 'T') { //translational sweep
		objectType = 0;
	}
	else { // rotational sweep.
		objectType = 1;
	}

}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 2_26990940", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	glfwSetWindowSizeCallback(window, window_size_callback);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Build and compile our shader program
	// Vertex shader

	// Read the Vertex Shader code from the file
	string vertex_shader_path = "vertex.shader";
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = "fragment.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	// Set up the Camera
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	glm::mat4 view;
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*arraySize, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices)*indicesSize, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	triangle_scale = glm::vec3(1.0f); //shorthand, initializes all 4 compenents to 1.0f

	GLuint mm = glGetUniformLocation(shaderProgram, "model_matrix");
	GLuint vm = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint pm = glGetUniformLocation(shaderProgram, "proj_matrix");

	float rotation = 9;

	askForInput();

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);
		model_matrix = glm::rotate(model_matrix, glm::radians(xValue), glm::vec3(1, 0, 0));
		model_matrix = glm::rotate(model_matrix, glm::radians(y), glm::vec3(0, 1, 0));
		model_matrix = glm::rotate(model_matrix, glm::radians(zValue), glm::vec3(0, 0, 1));

		view_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, cameraZoom),	 // camera positioned here
			glm::vec3(0.0f, 0.0f, 0.0f), // looks at origin
			glm::vec3(0.0f, 1.0f, 0.0f));// up vector

		glm::mat4 proj_matrix;
		proj_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);


		glUniformMatrix4fv(mm, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(vm, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(pm, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, projPoints->size());
		//glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
