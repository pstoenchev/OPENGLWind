#include <iostream>
#include <memory>

#pragma comment(lib, "glfw3.lib")

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void on_error(int code, const char* text);

const GLuint WIDTH = 800, HEIGHT = 600;

int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();

	float tx = -0.5, ty = 0.5, tz = 0.0;
	bool incx = true, incy = false;
	float zTrans = 0.f;
	int direct = -1;
	float angl = 80.f;
	float angldir = 0.1;
	try
	{
		stbi_set_flip_vertically_on_load(true);// Tell stb to filp images so that 0.0 is at the bottom of the y-axis

		glfwSetErrorCallback(on_error);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", NULL, NULL);
		glfwMakeContextCurrent(window);
		if (window == NULL)
			throw std::exception("Failed to create GLFW window");

		glfwSetKeyCallback(window, key_callback);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			throw std::exception("Failed to initialize OpenGL context");

		// Define the viewport dimensions
		glViewport(0, 0, WIDTH, HEIGHT);

		// Vertex buffer object to draw
		struct Vertex
		{
			float x, y, z;
			float r, g, b;
			float s, t;
			Vertex(float x, float y, float z, float r, float g, float b, float s, float t)
				:x(x), y(y), z(z), r(r), g(g), b(b), s(s), t(t) {}
		};
		// Coordinates are in NDC (Normalized Device Coordinates) from -1 to 1
		Vertex vertices[] = {
			Vertex(0.5f, 0.5f, 0.0f, .00f, .50f, .20f, 0.0f, 0.f ),
			Vertex( 0.5f, 0.5f, 0.0f, .00f, .50f, .20f, 1.0f, 0.f ),
			Vertex(-0.5f,  0.5f, 0.0f, .00f, .50f, .20f, 0.0f, 1.f),
			Vertex( -0.5f,  -0.5f, 0.0f, .00f, .50f, .20f, 1.0f, 1.f ),



			Vertex(-0.5f, -0.5f, 0.0f, .00f, .50f, .20f, 0.0f, 0.f),
			Vertex(-0.5f, -0.5f, 0.0f, .00f, .50f, .20f, 1.0f, 0.f),  // 2 triangle
			Vertex(0.5f,  -0.5f, 0.0f, .00f, .50f, .20f, 0.0f, 1.f),
			Vertex(0.5f,  0.5f, 0.0f, .00f, .50f, .20f, 1.0f, 1.f)
			                                                                 //new


		};

		// A Vertex Array Object will store OpenGL's state 
		unsigned vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Create a Vertex Buffer Object
		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

#define PERSPECTIVE
#ifndef PERSPECTIVE
		ShaderProgram shader("Shaders/vertex.vert", "Shaders/multitextures.frag");
#else
		ShaderProgram shader("Shaders/mvp.vert", "Shaders/multitextures.frag");
#endif
		shader.use();

		// Map the vertex attributes
		glVertexAttribPointer(0 /*location=0*/, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		// Load the textures
		{
			int width, height, nrChannels;
			std::shared_ptr<unsigned char> pContainerTex = std::shared_ptr<unsigned char>(stbi_load("Resources/container.jpg", &width, &height, &nrChannels, 0), stbi_image_free);
			int width2, height2, nrChannels2;
			std::shared_ptr<unsigned char> pFaceTex = std::shared_ptr<unsigned char>(stbi_load("Resources/awesomeface.png", &width2, &height2, &nrChannels2, 0), stbi_image_free);
		
			if (!pContainerTex || !pFaceTex)
				throw std::exception("Failed to load texture");

			// Generate texture objects
			unsigned textures[2];
			glGenTextures(2, textures);

			// Activate Texture unit
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[0]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pContainerTex.get());
			glGenerateMipmap(GL_TEXTURE_2D);

			glActiveTexture(GL_TEXTURE1 /*GL_TEXTURE0 + 1*/);
			glBindTexture(GL_TEXTURE_2D, textures[1]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pFaceTex.get());
			glGenerateMipmap(GL_TEXTURE_2D);

			// Assign textures to texture samplers in the fragment shader
			glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
			glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);
		}		
		double startTime = glfwGetTime();
		double start = glfwGetTime();
		float rotAngle = 0.f;
		while (!glfwWindowShouldClose(window))
		{

			//double current = glfwGetTime();
			//double dt1 = current - startTime
			//rotAngle = dt1*45.f;

			glfwPollEvents();

	



			// Render
			// Clear the colorbuffer
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

#ifndef PERSPECTIVE
			glm::mat4 trans = glm::mat4(1.0f);
			trans = glm::scale(trans, glm::vec3((float)HEIGHT / WIDTH, 1., 1.));
			trans = glm::rotate(trans, glm::radians(rotAngle), glm::vec3(0.f, 0.f, 1.f));
			trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.f));
			//trans = glm::rotate(trans, glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f));
			//trans = glm::rotate(trans, glm::radians((float)glfwGetTime()), glm::vec3(0.f, 0.f, 1.f));
			trans = glm::rotate(trans, glm::radians(rotAngle), glm::vec3(0.f, 0.f, 1.f));
			trans = glm::scale(trans, glm::vec3(0.5, 0.5, 1.));

			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
#else // LATER
			// Model
			glm::mat4 model;
			model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			// View
			glm::mat4 view;
			// note that we're translating the scene in the reverse direction of where we want to move
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

			// Projection
			glm::mat4 projection;
			projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);


			model = glm::translate(model, glm::vec3(0.5f, -0.5f, 0.f));
			
			if (angl >= 100)
			{
				angldir = -0.1f;
			}
			else if (angl <= 80)
			{
				angldir = 0.1f;
			}
			angl = angl + (float)angldir;
			model = glm::rotate(model, glm::radians(angl), glm::vec3(1.f, 0.f, 0.f));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 1.));

			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
#endif

			GLenum mode = GL_TRIANGLE_STRIP; // GL_TRIANGLES GL_POINTS GL_LINE_STRIP GL_LINE_LOOP
			glDrawArrays(mode, 0 /*start index in the vbo*/, sizeof(vertices) / sizeof(vertices[0]) /*number of vertices*/);
			
			double current = glfwGetTime();
			double dt1 = current - startTime;
			//startTime = current;
			rotAngle += dt1;
			zTrans = zTrans + (0.01*direct);
			if (zTrans >= 0.f)
				direct = -1;
			else if (zTrans <= -50.f)
			{
				direct = 1;
				zTrans = 4.f;
			}

			model = glm::mat4(1.f);
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(.5f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(-0.9f, 0.5f, zTrans));
			//model = glm::rotate(model, glm::radians(angl - 10.f), glm::vec3(1.5f, -0.7f, -0.5f));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(mode, 1 /*start index in the vbo*/, sizeof(vertices) / sizeof(vertices[0]) /*number of vertices*/);





			// Swap the screen buffers
			glfwSwapBuffers(window);
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Unexpected error: " << e.what() << std::endl;
		std::cin.get();
	}

	// Terminates GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void on_error(int code, const char* text)
{
	std::cout << "Error code: " << code << " Error text: " << text << std::endl;
}
