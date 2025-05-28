#include <iostream>
#include <sstream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// GLAD must be included before GLFW, since glad.h also includes gl.h (which GLFW needs)
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Main.h"

#include "Shader.h"
#include "Camera.h"
#include "Util.h"

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
	glm::vec3( 0.0f,  0.0f,  0.0f), 
	glm::vec3( 2.0f,  5.0f, -15.0f), 
	glm::vec3(-1.5f, -2.2f, -2.5f),  
	glm::vec3(-3.8f, -2.0f, -12.3f),  
	glm::vec3( 2.4f, -0.4f, -3.5f),  
	glm::vec3(-1.7f,  3.0f, -7.5f),  
	glm::vec3( 1.3f, -2.0f, -2.5f),  
	glm::vec3( 1.5f,  2.0f, -2.5f), 
	glm::vec3( 1.5f,  0.2f, -1.5f), 
	glm::vec3(-1.3f,  1.0f, -1.5f)  
};

glm::vec3 pointLightPositions[] = {
	glm::vec3( 0.7f,  0.2f,  2.0f),
	glm::vec3( 2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3( 0.0f,  0.0f, -3.0f)
};

// Global access to material and light properties for real-time editing.
struct Material
{
	float shininess = 32.0f;
	float emissiveStrength = 0.0f;
};

struct Light
{
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
};

struct DirectionalLight : Light
{
	glm::vec3 direction;
};

struct PointLight : Light
{
	glm::vec3 position;
	float constant = 1.0f;
	float linear;
	float quadratic;
};

struct SpotLight : Light
{
	glm::vec3 position;
	glm::vec3 direction;
	float constant = 1.0f;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

struct Material material;
struct DirectionalLight directionalLight;
#define NR_POINT_LIGHTS 4
struct PointLight pointLights[NR_POINT_LIGHTS];
struct SpotLight spotLight;
glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
bool wireframe = false;


int windowWidth = 1600, windowHeight = 900;

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Global timing variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Mouse movement variables
bool firstMouseInput = true; // To prevent a jarring "jump" when the player first moves the mouse.
float lastX = windowWidth / 2.0, lastY = windowHeight / 2.0;

int main()
{
	// GLFW and GLAD init.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return 1;
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set up callbacks to handle mouse movement and scrolling.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// Enable depth testing with the z-buffer.
	glEnable(GL_DEPTH_TEST);


	
	// ImGUI init.
	// Set up Dear ImGui context.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Set up renderer/platform backends.
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();



	// Load and create textures
	// OpenGL's y coordinates increase upwards, whereas a picture's y coordinates increase downwards.
	stbi_set_flip_vertically_on_load(true);
	
	unsigned int diffuseMap = loadTexture("resources\\container2.png");
	unsigned int specularMap = loadTexture("resources\\container2_specular.png");
	unsigned int emissiveMap = loadTexture("resources\\matrix.jpg");


	
	// Set up vertex and fragment shaders for meshes and light source mesh.
	Shader lightingShader = Shader("shaders\\lighting.vsh", "shaders\\lighting.fsh");
	Shader lightSourceShader = Shader("shaders\\lightsource.vsh", "shaders\\lightsource.fsh");


	
	// Vertex setup
	unsigned int VAO, VBO;

	// Bind Vertex array object (VAO) to tell OpenGL how to fill input data for the vertex shader.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create a vertex buffer object (VBO) for storing our vertices on the GPU.
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable vertex attrib pointers (vec3 pos, vec3 normal, vec2 texCoords)
	VertexAttribBuilder meshAttribBuilder = VertexAttribBuilder();
	meshAttribBuilder.AddAttribute(3, GL_FLOAT);
	meshAttribBuilder.AddAttribute(3, GL_FLOAT);
	meshAttribBuilder.AddAttribute(2, GL_FLOAT);
	meshAttribBuilder.Finalize();


	
	// Light VAO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Can't use my new VertexAttribBuilder since the stride is custom (some data is skipped). 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	
	// Light initialization.
	directionalLight.direction = glm::vec3(1.0f, -1.0f, 1.0f);

	for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		pointLights[i].position = pointLightPositions[i];
		pointLights[i].linear = 0.09f;
		pointLights[i].quadratic = 0.032f;
	}

	spotLight.linear = 0.09f;
	spotLight.quadratic = 0.032f;
	spotLight.cutOff = 12.5f;
	spotLight.outerCutOff = 17.5f;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Delta time calculation
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		// Input
		processInput(window);

		// Start Dear ImGui frame.
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Construct my own very awesome debug window.
		ImGui::Begin("Scene Settings");
		if (ImGui::Button("Toggle Wireframe"))
		{
			wireframe = !wireframe;
			if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
			
		if (ImGui::CollapsingHeader("Scene Colors"))
		{
			ImGui::ColorEdit4("Background Color", (float*)&clearColor);
			
			if (ImGui::TreeNode("Material"))
			{
				ImGui::SliderFloat("Shininess", &material.shininess, 0.1, 64.0);
				ImGui::SliderFloat("Emissive Strength", &material.emissiveStrength, 0.0, 1.0);
				
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Directional Light"))
			{
				ImGui::SliderFloat3("Direction", (float*)&directionalLight.direction, -1.0f, 1.0f);
				ImGui::ColorEdit3("Color", (float*)&directionalLight.color);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Point Lights"))
			{
				for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
				{
					std::ostringstream ss;
					ss << "Point Light #" << i + 1;

					if (ImGui::TreeNode(ss.str().c_str()))
					{
						ImGui::SliderFloat3("Position", (float*)&pointLights[i].position, -20.0f, 20.0f);
						ImGui::ColorEdit3("Color", (float*)&pointLights[i].color);
						ImGui::SliderFloat("Linear Falloff", &pointLights[i].linear, 0.0f, 0.5f);
						ImGui::SliderFloat("Quadratic Falloff", &pointLights[i].quadratic, 0.0f, 0.5f);
						
						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Spot Light"))
			{
				ImGui::ColorEdit3("Color", (float*)&spotLight.color);
				ImGui::SliderFloat("Inner Cone Angle", &spotLight.cutOff, 1.0f, 89.0f);
				ImGui::SliderFloat("Outer Cone Angle", &spotLight.outerCutOff, 1.0f, 89.0f);
				ImGui::SliderFloat("Linear Falloff", &spotLight.linear, 0.0f, 1.0f);
				ImGui::SliderFloat("Quadratic Falloff", &spotLight.quadratic, 0.0f, 1.0f);

				ImGui::TreePop();
			}
		}
		char* frameTime = new char[32];
		sprintf_s(frameTime, 32, "%.2f FPS / %.2f ms", 1.0f / deltaTime, deltaTime * 1000.0f);
		ImGui::Text(frameTime);
		ImGui::End();

		// Rendering
		// Clear the color and depth buffers from the previous frame.
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind textures to texture units/targets for the samplers to use.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissiveMap);

		// Set lighting-related values for our fragment shader.
		lightingShader.use();
		// Use texture unit 0 as sampler source for the diffuse map and so on.
		lightingShader.setInt("material.diffuse", 0);
		lightingShader.setInt("material.specular", 1);
		lightingShader.setInt("material.emissive", 2);
		lightingShader.setFloat("material.shininess", material.shininess);
		lightingShader.setFloat("material.emissiveStrength", material.emissiveStrength);

		// Directional Light attributes.
		lightingShader.setVec3("directionalLight.direction", directionalLight.direction);
		lightingShader.setVec3("directionalLight.ambient", directionalLight.color * 0.1f);
		lightingShader.setVec3("directionalLight.diffuse", directionalLight.color * 0.5f);
		lightingShader.setVec3("directionalLight.specular", directionalLight.color);

		// Point Light attributes.
		for (int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			std::ostringstream ss1;
			ss1 << "pointLights[" << i << "].position";
			lightingShader.setVec3(ss1.str(), pointLights[i].position);
			
			std::ostringstream ss2;
			ss2 << "pointLights[" << i << "].ambient";
			lightingShader.setVec3(ss2.str(), pointLights[i].color * 0.1f);

			std::ostringstream ss3;
			ss3 << "pointLights[" << i << "].diffuse";
			lightingShader.setVec3(ss3.str(), pointLights[i].color * 0.5f);

			std::ostringstream ss4;
			ss4 << "pointLights[" << i << "].specular";
			lightingShader.setVec3(ss4.str(), pointLights[i].color);

			std::ostringstream ss5;
			ss5 << "pointLights[" << i << "].constant";
			lightingShader.setFloat(ss5.str(), pointLights[i].constant);

			std::ostringstream ss6;
			ss6 << "pointLights[" << i << "].linear";
			lightingShader.setFloat(ss6.str(), pointLights[i].linear);

			std::ostringstream ss7;
			ss7 << "pointLights[" << i << "].quadratic";
			lightingShader.setFloat(ss7.str(), pointLights[i].quadratic);
		}

		// Spotlight attributes.
		spotLight.position = camera.Position;
		spotLight.direction = camera.Front;
		lightingShader.setVec3("spotLight.position", spotLight.position);
		lightingShader.setVec3("spotLight.direction", spotLight.direction);
		lightingShader.setVec3("spotLight.ambient", spotLight.color * 0.1f);
		lightingShader.setVec3("spotLight.diffuse", spotLight.color * 0.5f);
		lightingShader.setVec3("spotLight.specular", spotLight.color);
		lightingShader.setFloat("spotLight.constant", spotLight.constant);
		lightingShader.setFloat("spotLight.linear", spotLight.linear);
		lightingShader.setFloat("spotLight.quadratic", spotLight.quadratic);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(spotLight.cutOff)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLight.outerCutOff)));

		// Defining view matrices (model [not here], view, projection) to transform vertices to NDC.
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

		// Send transformation matrices to shader. Send them every frame since they tend to change often.
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("projection", projection);

		// Draw multiple cubes, each with a different offset and rotation over time.
		glBindVertexArray(VAO);
		for(unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Configure the light source shader.
		lightSourceShader.use();
		lightSourceShader.setMat4("view", view);
		lightSourceShader.setMat4("projection", projection);

		// Draw the light source cubes.
		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLights[i].position);
			model = glm::scale(model, glm::vec3(0.2f));
			lightSourceShader.setMat4("model", model);

			lightSourceShader.setVec3("lightColor", pointLights[i].color);
			
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Unbind the vertex array after it has been used.
		glBindVertexArray(0);

		// ImGui: Render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// GLFW: swap buffers and poll input events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// De-allocate all arrays and buffers when the program terminates.
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// Shut down Dear ImGui.
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, windowWidth, windowHeight);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// To prevent first-time input "jump".
	if (firstMouseInput)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseInput = false;
	}
	
	// Calculate mouse offset between last frame and this one.
	float offsetX = xpos - lastX;
	float offsetY = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(offsetX, offsetY);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		camera.SetControlMode(!camera.Controlled);
		if (camera.Controlled)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}