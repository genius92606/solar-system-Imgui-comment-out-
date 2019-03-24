
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#define GLM_FORCE_RADIAN
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>
#include <imgui.h>
#include <memory>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "WenCheng.h"


static void error_callback(int error, const char* description)
{
    std::cerr<<fmt::format("Error: {0}\n", description);
}

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    // Good bye Mac OS X
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1000, 800, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGL()) {
        exit(EXIT_FAILURE);
    }
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Setup style
    ImGui::StyleColorsDark();

    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

    auto text_sun = Texture2D::LoadFromFile("../resource/sun.png");
	auto text_earth = Texture2D::LoadFromFile("../resource/earth.png");
	auto text_moon = Texture2D::LoadFromFile("../resource/moon.png");
    auto sphere = StaticMesh::LoadMesh("../resource/sphere.obj");
    auto prog = Program::LoadFromFile("../resource/vs.vert", "../resource/fs.frag");


    // Do not remove {}, why???
	{
		// text and mesh, shader => garbage collector
		auto g1 = Protect(text_sun);
		auto g2 = Protect(text_earth);
		auto g3 = Protect(text_moon);
		auto g4 = Protect(sphere);
		auto g6 = Protect(prog);
		

		if (!sphere.hasNormal() || !sphere.hasUV()) {
			std::cerr << "mesh_sun does not have normal or uv\n";
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}


		float degree = 0.0f;

		float degree_earth_rev = 0.0f;   //earth revolution
		float degree_earth_rot = 0.0;    //earth rotation
		float degree_moon_rev = 0.0f;    //moon revolution
		float degree_moon_rot = 0.0f;    //moon rotation

		float sun_to_earth = 4.0f;    //the distance from sun to earth
		float earth_to_moon = 0.7f;    //the distance from earth to moon
		glm::vec3 object_color{ 1.0f };

		while (!glfwWindowShouldClose(window))
		{
			degree++;
			glfwPollEvents();

			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);

			prog["vp"] = glm::perspective(45 / 180.0f*3.1415926f, 640.0f / 480.0f, 0.1f, 10000.0f)*
				glm::lookAt(glm::vec3{ 0, 0, 10 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });

			//sun///////////

			prog["model"] =glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			//prog["model"] = glm::rotate(glm::mat4(1.0f), degree*3.1415926f / 180.0f, glm::vec3(0, 1, 0));
			prog["object_color"] = object_color;
			
			prog["text"] = 0;
			text_sun.bindToChannel(0);
			prog.use();
			sphere.draw();


			//earth//////////

			prog["model"] = glm::rotate(glm::mat4(1.0f), degree  * 3.1415926f / 180.0f, glm::vec3(0, 1, 0))*
				glm::translate(glm::mat4(1.0f), glm::vec3(sun_to_earth, 0.0f, 0.0f))*
				glm::scale(glm::mat4(1.0f), glm::vec3(0.4f))*
				glm::rotate(glm::mat4(1.0f), degree/2*3.1415926f / 180.0f, glm::vec3(0, 1, 0));
			prog["object_color"] = object_color;

			text_earth.bindToChannel(0);
			prog.use();
			sphere.draw();


			//moon///////////

			prog["model"] = 
				glm::rotate(glm::mat4(1.0f), degree  * 3.1415926f / 180.0f, glm::vec3(0, 1, 0))*
				glm::translate(glm::mat4(1.0f), glm::vec3(sun_to_earth, 0.0f, 0.0f))*
				glm::rotate(glm::mat4(1.0f), degree*5 * 3.1415926f / 180.0f, glm::vec3(0, 1, 0))*
				glm::translate(glm::mat4(1.0f), glm::vec3(earth_to_moon, 0.0f, 0.0f))*
				glm::scale(glm::mat4(1.0f), glm::vec3(0.1f))*
				glm::rotate(glm::mat4(1.0f), degree*15*3.1415926f / 180.0f, glm::vec3(0, 1, 0));
			prog["object_color"] = object_color;

			text_moon.bindToChannel(0);
			prog.use();
			sphere.draw();

			glDisable(GL_DEPTH_TEST);


			/*
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			bool new_shader = false;
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			{
				static int counter = 0;

				ImGui::Begin("fucking bullshit!!!!!!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::SliderFloat("degree", &degree, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
				ImGui::ColorEdit3("object color", glm::value_ptr(object_color)); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);
				ImGui::Image(reinterpret_cast<ImTextureID>(text_sun.id()), ImVec2{ 128, 128 });
				ImGui::Image(reinterpret_cast<ImTextureID>(text_earth.id()), ImVec2{ 128, 128 });
				ImGui::Image(reinterpret_cast<ImTextureID>(text_moon.id()), ImVec2{ 128, 128 });
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				if (ImGui::Button("Reload Shader")) {
					auto new_prog = Program::LoadFromFile("../resource/vs.vert", "../resource/fs.frag");
					// because we would like to switch value of prog
					// we need to release object on our own.
					prog.release();
					prog = new_prog;

				}
				ImGui::End();
			}


			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			*/
			glfwSwapBuffers(window);

		}


	}
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
