#include <iostream>
#include <sstream>
#include <string>

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
#include "Model.h"

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

float ambientMultiplier = 0.1f;
float diffuseMultiplier = 0.5f;
float specularMultiplier = 1.0f;

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

	// Add model shaders and model itself.
	Shader modelShader = Shader("shaders\\model.vsh", "shaders\\model.fsh");
	Model backpack = Model("resources\\backpack.obj");


	
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

			ImGui::SliderFloat("Ambient Multiplier", &ambientMultiplier, 0.0f, 1.0f);
			ImGui::SliderFloat("Diffuse Multiplier", &diffuseMultiplier, 0.0f, 1.0f);
			ImGui::SliderFloat("Specular Multiplier", &specularMultiplier, 0.0f, 1.0f);
			
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

		// Defining view matrices (model, view, projection) to transform vertices to NDC.
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);

		// Send transformation matrices to shader. Send them every frame since they tend to change often.
		modelShader.use();
		modelShader.setMat4("model", model);
		modelShader.setMat4("view", view);
		modelShader.setMat4("projection", projection);

		// Send material and lighting information to shader.
		modelShader.setFloat("material.shininess", material.shininess);

		// Directional Light attributes.
		modelShader.setVec3("directionalLight.direction", directionalLight.direction);
		modelShader.setVec3("directionalLight.ambient", directionalLight.color * ambientMultiplier);
		modelShader.setVec3("directionalLight.diffuse", directionalLight.color * diffuseMultiplier);
		modelShader.setVec3("directionalLight.specular", directionalLight.color * specularMultiplier);

		// Point Light attributes.
		for (int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			std::string index = std::to_string(i);
			
			modelShader.setVec3("pointLights[" + index + "].position", pointLights[i].position);
			modelShader.setVec3("pointLights[" + index + "].ambient", pointLights[i].color * ambientMultiplier);
			modelShader.setVec3("pointLights[" + index + "].diffuse", pointLights[i].color * diffuseMultiplier);
			modelShader.setVec3("pointLights[" + index + "].specular", pointLights[i].color * specularMultiplier);
			modelShader.setFloat("pointLights[" + index + "].constant", pointLights[i].constant);
			modelShader.setFloat("pointLights[" + index + "].linear", pointLights[i].linear);
			modelShader.setFloat("pointLights[" + index + "].quadratic", pointLights[i].quadratic);
		}

		// Spotlight attributes.
		spotLight.position = camera.Position;
		spotLight.direction = camera.Front;
		modelShader.setVec3("spotLight.position", spotLight.position);
		modelShader.setVec3("spotLight.direction", spotLight.direction);
		modelShader.setVec3("spotLight.ambient", spotLight.color * ambientMultiplier);
		modelShader.setVec3("spotLight.diffuse", spotLight.color * diffuseMultiplier);
		modelShader.setVec3("spotLight.specular", spotLight.color * specularMultiplier);
		modelShader.setFloat("spotLight.constant", spotLight.constant);
		modelShader.setFloat("spotLight.linear", spotLight.linear);
		modelShader.setFloat("spotLight.quadratic", spotLight.quadratic);
		modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(spotLight.cutOff)));
		modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLight.outerCutOff)));

		// Draw our 3D model!
		backpack.Draw(modelShader);

		// ImGui: Render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// GLFW: swap buffers and poll input events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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