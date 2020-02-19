#include "gameloop.h"

#include "../Context/context.h"
#include "../Rendering/graphic.h"
#include "../Rendering/shader.h"
#include "../Rendering/texture.h"
#include "../Input/input_responder.h"
#include "../Utils/miscellaneous.h"
#include "../Rendering/ImGUI/editor.h"
#include "../Utils/time.h"



bool gameloop::run(int argc, char* argv[]) {

	std::filesystem::path exeFile = argv[0];
	environment::exeDirectory = exeFile.parent_path();
	environment::resourcesPath = environment::exeDirectory.parent_path() / "Resources";

	/*----------------------------------------------------------------------------------*/

	// Context

	context::initialiseGLFW();

	bool success = context::window::setupWindow("My Game", 600, 450);
	if (!success) {
		return false;
	}

	success = context::graphics::initialiseGraphics();
	if (!success) {
		return false;
	}

	context::imguiContext();

	/*----------------------------------------------------------------------------------*/

	{
		Shader shader(environment::ResourcePath("Shaders/shader.vs"), environment::ResourcePath("Shaders/shader.fs"));

		/*----------------------------------------------------------------------------------*/

		float vertices[] = {
		   -0.5f, -0.5f, -0.5f,  0.f, 0.f,
			0.5f, -0.5f, -0.5f,  1.f, 0.f,
			0.5f,  0.5f, -0.5f,  1.f, 1.f,
			0.5f,  0.5f, -0.5f,  1.f, 1.f,
		   -0.5f,  0.5f, -0.5f,  0.f, 1.f,
		   -0.5f, -0.5f, -0.5f,  0.f, 0.f,

		   -0.5f, -0.5f,  0.5f,  0.f, 0.f,
			0.5f, -0.5f,  0.5f,  1.f, 0.f,
			0.5f,  0.5f,  0.5f,  1.f, 1.f,
			0.5f,  0.5f,  0.5f,  1.f, 1.f,
		   -0.5f,  0.5f,  0.5f,  0.f, 1.f,
		   -0.5f, -0.5f,  0.5f,  0.f, 0.f,

		   -0.5f,  0.5f,  0.5f,  1.f, 0.f,
		   -0.5f,  0.5f, -0.5f,  1.f, 1.f,
		   -0.5f, -0.5f, -0.5f,  0.f, 1.f,
		   -0.5f, -0.5f, -0.5f,  0.f, 1.f,
		   -0.5f, -0.5f,  0.5f,  0.f, 0.f,
		   -0.5f,  0.5f,  0.5f,  1.f, 0.f,

			0.5f,  0.5f,  0.5f,  1.f, 0.f,
			0.5f,  0.5f, -0.5f,  1.f, 1.f,
			0.5f, -0.5f, -0.5f,  0.f, 1.f,
			0.5f, -0.5f, -0.5f,  0.f, 1.f,
			0.5f, -0.5f,  0.5f,  0.f, 0.f,
			0.5f,  0.5f,  0.5f,  1.f, 0.f,

		   -0.5f, -0.5f, -0.5f,  0.f, 1.f,
			0.5f, -0.5f, -0.5f,  1.f, 1.f,
			0.5f, -0.5f,  0.5f,  1.f, 0.f,
			0.5f, -0.5f,  0.5f,  1.f, 0.f,
		   -0.5f, -0.5f,  0.5f,  0.f, 0.f,
		   -0.5f, -0.5f, -0.5f,  0.f, 1.f,

		   -0.5f,  0.5f, -0.5f,  0.f, 1.f,
			0.5f,  0.5f, -0.5f,  1.f, 1.f,
			0.5f,  0.5f,  0.5f,  1.f, 0.f,
			0.5f,  0.5f,  0.5f,  1.f, 0.f,
		   -0.5f,  0.5f,  0.5f,  0.f, 0.f,
		   -0.5f,  0.5f, -0.5f,  0.f, 1.f
		};

		glm::vec3 cube_positions[] = {
			glm::vec3(-2.f, -2.f, -5.f),
			glm::vec3(0.f, -2.f, -5.f),
			glm::vec3(2.f, -2.f, -5.f),
			glm::vec3(-2.f, 0.f, -5.f),
			glm::vec3(0.f, 0.f, -5.f),
			glm::vec3(2.f, 0.f, -5.f),
			glm::vec3(-2.f, 2.f, -5.f),
			glm::vec3(0.f, 2.f, -5.f),
			glm::vec3(2.f, 2.f, -5.f),
		};

		std::vector<unsigned int> indices = {};

		/*----------------------------------------------------------------------------------*/

		unsigned int VBO, VAO;
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		VertAttribObject vertex_and_attrib_object(VAO, VBO);

		vertex_and_attrib_object.bindVBO(vertices, sizeof(vertices), VBO);
		vertex_and_attrib_object.bindVAO(VAO);

		vertex_and_attrib_object.positionAttrib();
		vertex_and_attrib_object.textureCoordAttrib();

		/*----------------------------------------------------------------------------------*/

		// Textures

		std::vector<std::string> textures = readFile("../Resources/DirectoryReader/textures_list.txt");

		Texture transparent1(environment::ResourcePath("Textures/T_Transparent/graffiti_texture1.png"));

		auto texture1 = std::make_unique<Texture>(environment::ResourcePath("Textures/T_Metal/metal_bricks1.jpg"));
		auto texture2 = std::make_unique<Texture>(environment::ResourcePath("Textures/T_Wood/wood_planks1.jpg"));
		auto texture3 = std::make_unique<Texture>(environment::ResourcePath("Textures/T_Rock/natural_rock1.jpg"));

		Texture error_texture(environment::ResourcePath("Textures/error_texture1.png"));

		std::vector<std::unique_ptr<Texture>> loaded_textures;

		loaded_textures.push_back(NULL);
		loaded_textures.push_back(std::move(texture1));
		loaded_textures.push_back(std::move(texture2));
		loaded_textures.push_back(std::move(texture3));

		shader.use();
		shader.setInt("transparent1", 1);

		/*----------------------------------------------------------------------------------*/

		// Cubes

		auto transform1 = std::make_unique<Transform>(0, cube_positions[0], glm::quat(0.f, 1.0f, 0.3f, 0.5f));
		auto transform2 = std::make_unique<Transform>(1, cube_positions[1], glm::quat(0.f, 1.0f, 0.3f, 0.5f));
		auto transform3 = std::make_unique<Transform>(2, cube_positions[2], glm::quat(0.f, 1.0f, 0.3f, 0.5f));
		auto transform4 = std::make_unique<Transform>(3, cube_positions[3], glm::quat(0.f, 1.0f, 0.3f, 0.5f));
		auto transform5 = std::make_unique<Transform>(4, cube_positions[4], glm::quat(0.f, 1.0f, 0.3f, 0.5f));
		auto transform6 = std::make_unique<Transform>(5, cube_positions[5], glm::quat(0.f, 1.0f, 0.3f, 0.5f));
		auto transform7 = std::make_unique<Transform>(6, cube_positions[6], glm::quat(0.f, 1.0f, 0.3f, 0.5f));
		auto transform8 = std::make_unique<Transform>(7, cube_positions[7], glm::quat(0.f, 1.0f, 0.3f, 0.5f));
		auto transform9 = std::make_unique<Transform>(8, cube_positions[8], glm::quat(0.f, 1.0f, 0.3f, 0.5f));

		std::vector<std::unique_ptr<Transform>> cube_position_objects;

		cube_position_objects.push_back(std::move(transform1));
		cube_position_objects.push_back(std::move(transform2));
		cube_position_objects.push_back(std::move(transform3));
		cube_position_objects.push_back(std::move(transform4));
		cube_position_objects.push_back(std::move(transform5));
		cube_position_objects.push_back(std::move(transform6));
		cube_position_objects.push_back(std::move(transform7));
		cube_position_objects.push_back(std::move(transform8));
		cube_position_objects.push_back(std::move(transform9));

		/*----------------------------------------------------------------------------------*/

		Camera camera(-90.f, 0.f, (800.f / 2.f), (600.f / 2.f), 45.f, 100.f);

		Time last_frame = Time::now();

		/*----------------------------------------------------------------------------------*/

		// Game loop
		while (context::window::isClosed(context::window::getWindow()) == false) {

			glPolygonMode(GL_FRONT_AND_BACK, polygon_mode ? GL_LINE : GL_FILL);

			// Initialise DT
			Time delta_time = Time::now() - last_frame;
			last_frame = Time::now();

			camera.update(delta_time.getSeconds());

#ifdef DEBUG_MODE
			// Render imGUI
			context::createImguiWindow("My GUI###GUI1");

			if (ImGui::BeginTabBar("###tab_bar1")) {

				if (ImGui::BeginTabItem("Debug###debug1")) {
					collapsingHeader::texture(textures, std::move(loaded_textures), error_texture, transparent1, false);
					collapsingHeader::colour(false);
					collapsingHeader::miscellaneous(false);

					ImGui::TextWrapped("\n");

					ImGui::TextWrapped("\Ping: %lf", delta_time.getMilliseconds());
					ImGui::TextWrapped("\nFPS: %lf", framesPerSecond(delta_time.getSeconds()));

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Editor###editor1")) {
					ImGui::TextWrapped("\n");

					collapsingHeader::texture(textures, std::move(loaded_textures), error_texture, transparent1, true);
					collapsingHeader::colour(true);
					collapsingHeader::miscellaneous(true);

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Help###help1")) {
					collapsingHeader::texture(textures, std::move(loaded_textures), error_texture, transparent1, false);
					collapsingHeader::colour(false);
					collapsingHeader::miscellaneous(false);

					ImGui::TextWrapped("\n");

					collapsingHeader::controlsText(true);
					collapsingHeader::aboutText(true);

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
			else {
				collapsingHeader::texture(textures, std::move(loaded_textures), error_texture, transparent1, true);
				collapsingHeader::colour(true);
				collapsingHeader::miscellaneous(true);
			}

			bool is_mouse_over_ui;
			if (ImGui::IsWindowHovered()) {
				is_mouse_over_ui = true;
			}
			else {
				is_mouse_over_ui = false;
			}

			ImGui::End();
#endif

			// Projection + View + Transform [MATRICES]
			glm::mat4 projection = camera.getMat4Projection();
			shader.setMat4("projection", projection);

			glm::mat4 view = camera.getMat4View();
			shader.setMat4("view", view);

			glm::mat4 transform = camera.getMat4Transform();
			unsigned int transform_loc = glGetUniformLocation(shader.mID, "transform");
			shader.modMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));

			// Renders boxes
			for (size_t i = 0; i < cube_position_objects.size(); i++) {
				glm::mat4 model = cube_position_objects[i]->getModel();
				shader.setMat4("model", model);

				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			// Unbind all texture units 
			std::vector<unsigned int> texture_units = { 0, 1, 2 };
			Texture::unbind(texture_units);


#ifdef DEBUG_MODE
			context::renderImgui();
#endif
			context::window::swapBuffers();
			context::window::pollEvents();
			update(delta_time.getSeconds(), camera, is_mouse_over_ui);
			input::endFrame();
		}
	}

	glfwTerminate();
	return true;
}
