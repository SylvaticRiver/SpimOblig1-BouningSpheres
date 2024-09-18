
#include"Mesh.h"
#include "Object.h"
#include "PhysicsObject.h"
#include "Shape.h"
#include "QuadTre.h"
#include <list>
#include <array>

int windowWidth = 1280;
int windowHeight = 720;

//Trykk ENTER for aa starte opp ballene fra resting state
//hvis ballene begynner aa gaa for sakte kan du trykke ENTER igjen for aa gi dem ny momentum

GLFWwindow* window;

bool wireframe = false;
bool showLights = true;
bool dotRender = false;
bool start = false;

void processInput(GLFWwindow* window, Camera& camera) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
		cout << "I have been closed" << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		wireframe = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		wireframe = false;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		showLights = false;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		showLights = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		dotRender = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		dotRender = false;
	}
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		start = true;
	}
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
		start = false;
	}
	camera.Inputs(window);
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	cout << "width = " << width << " height = " << height << endl;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Hello again", NULL, NULL);
	if (window == NULL) {
		cout << "Window was never created" << endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "failed to init GLAD" << endl;
		glfwTerminate();
	}

	Texture textures[]
	{
		Texture("empty_keg.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("empty_keg_spec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	QuadTre tre = QuadTre();
	tre.subDivide(1);

	Shader shaderProgram("Resources/Vertexshader.vert", "Resources/FragmentShader.frag");
	Shape cube(Shape::CUBE);
	Shape sph(Shape::SPHERE);
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

	Mesh cMesh(cube.getVerts(), cube.getInds(), tex);
	Mesh wMesh(cube.getVerts(), cube.getInds(), tex);
	Mesh sMesh(sph.getVerts(), sph.getInds(), tex);

	Shader lightShader("Resources/LightVert.vert", "Resources/LightFrag.frag");
	Shape lig(Shape::CUBE);
	Mesh lightMesh(lig.getVerts(), lig.getInds(), tex);

	LightObject light(lightMesh, glm::vec3(2.0f, 2.5f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f), Rotator{ glm::vec3(0.0f, 0.0f, 1.0f), 0.0f }, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), lightShader, shaderProgram);
	PhysicsObject* spheres[9] = {};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			PhysicsObject* a = new PhysicsObject(sMesh, glm::vec3((float)i - 1, 0.5, (float)j - 1), glm::vec3(0.1, 0.1, 0.1f), Rotator{ glm::vec3(0.0f, 0.0f, 1.0f), 0.0f }, shaderProgram, true, i * 0.2f);
			spheres[(i * 3) + j] = a;
			tre.insert(a);
		}
	}

	PhysicsObject plane(cMesh, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.1f, 5.0f), Rotator{ glm::vec3(0.0f, 0.0f, 1.0f), 0.0f }, shaderProgram, false, 1.0f);

	PhysicsObject wall1(wMesh, glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(2.5f, 2.5f, 0.1f), Rotator{ glm::vec3(0.0f, 0.0f, 1.0f), 0.0f }, shaderProgram, false, 1.0f);
	PhysicsObject wall2(wMesh, glm::vec3(0.0f, 0.0f, -2.5f), glm::vec3(2.5f, 2.5f, 0.1f), Rotator{ glm::vec3(0.0f, 0.0f, 1.0f), 0.0f }, shaderProgram, false, 1.0f);
	PhysicsObject wall3(wMesh, glm::vec3(2.5f, 0.0f, 0.0f), glm::vec3(0.1f, 2.5f, 2.5f), Rotator{ glm::vec3(0.0f, 0.0f, 1.0f), 0.0f }, shaderProgram, false, 1.0f);
	PhysicsObject wall4(wMesh, glm::vec3(-2.5f, 0.0f, 0.0f), glm::vec3(0.1f, 2.5f, 2.5f), Rotator{ glm::vec3(0.0f, 0.0f, 1.0f), 0.0f }, shaderProgram, false, 1.0f);

	PhysicsObject ball1(sMesh, glm::vec3((float)0 - 1, 2.0, (float)0 - 2), glm::vec3(0.1, 0.1, 0.1f), Rotator{ glm::vec3(0.0f, 0.0f, 1.0f), 0.0f }, shaderProgram, true, 1.0f);
	PhysicsObject ball2(sMesh, glm::vec3((float)1 - 1, 2.0, (float)2 - 1), glm::vec3(0.1, 0.1, 0.1f), Rotator{ glm::vec3(0.0f, 0.0f, 1.0f), 0.0f }, shaderProgram, true, 1.0f);

	glEnable(GL_DEPTH_TEST);

	Camera camera(windowWidth, windowHeight, glm::vec3(1.0f, 2.0f, -5.0));
	camera.orientation = glm::rotate(camera.orientation, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	while (!glfwWindowShouldClose(window))
	{
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		glClearColor(0.533, 0.996, 1, 0.78);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.Activate();
		camera.Matrix(shaderProgram, "cameraMatrix");
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
		processInput(window, camera);

		for (int i = 0; i < 9; i++) {
			spheres[i]->Render(shaderProgram, camera, dotRender);

			if (start) {
				spheres[i]->velocity.z = 0.05 * (rand() * 0.00005);
				spheres[i]->velocity.x = 0.03 * (rand() * 0.00005);
				spheres[i]->velocity.y = 0.04;
			}

			if (spheres[i]->isColliding((Object)plane)) {
				spheres[i]->onCollisionWall((Object)plane);
			}
			else if (spheres[i]->isColliding((Object)wall1)) {
				spheres[i]->onCollisionWall((Object)wall1);
			}
			else if (spheres[i]->isColliding((Object)wall2)) {
				spheres[i]->onCollisionWall((Object)wall2);
			}
			else if (spheres[i]->isColliding((Object)wall3)) {
				spheres[i]->onCollisionWall((Object)wall3);
			}
			else if (spheres[i]->isColliding((Object)wall4)) {
				spheres[i]->onCollisionWall((Object)wall4);
			}
		}

		//Provde meg paa per quad detection men fikk det ikke til, fikk lite tid siden jeg ble stuck paa ball-ball detection

		//for (QuadTre* quad : tre.quads) {
		//	if (quad->hasObjects()) {
		//		cout << quad->getObjects().size() << endl;
		//		for (int i = 0; i < quad->getObjects().size() - 1; i++) {
		//			for (int j = 0; j < quad->getObjects().size() - 1; j++) {
		//				PhysicsObject* o = quad->getObjects()[i];
		//				PhysicsObject* o2 = quad->getObjects()[j];
		//				if (quad->getObjects()[i]->isCollidingBall(quad->getObjects()[j]) && j != i) {
		//					quad->getObjects()[i]->onBallCollision(quad->getObjects()[j]);
		//				}
		//			}
		//		}
		//	}
		//}

		if (tre.hasObjects()) {
			for (int i = 0; i < tre.getObjects().size() - 1; i++) {
				for (int j = 0; j < tre.getObjects().size() - 1; j++) {
					if (tre.getObjects()[i]->isCollidingBall(tre.getObjects()[j]) && j != i) {
						tre.getObjects()[i]->onBallCollision(tre.getObjects()[j]);
					}
				}
			}
		}

		plane.Render(shaderProgram, camera, dotRender);
		wall1.Render(shaderProgram, camera, dotRender);
		wall2.Render(shaderProgram, camera, dotRender);
		wall3.Render(shaderProgram, camera, dotRender);
		wall4.Render(shaderProgram, camera, dotRender);

		if (showLights) {
			light.Render(lightShader, camera, dotRender);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	lightShader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
