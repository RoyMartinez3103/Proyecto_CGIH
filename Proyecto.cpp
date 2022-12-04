﻿/*---------------------------------------------------------*/
/* ----------------  Proyecto Computación Gráfica   -----------*/
/*-----------------    2023-1   ---------------------------*/
/*-------------  Martinez Zambrano Rodrigo ___ 316204707   ---------------*/
/*-------------  Perez Palumbo Fernando Alexis ___ 314185970 ---------------*/
/*-------------  Rodriguez Lopez Araiza Jose Eduardo ___ 315226742 --------------*/

//Se incluyen las librerias
#include <Windows.h>

#include <glad/glad.h>		//Permite cargar los modelos 3D
#include <glfw3.h>			//main
#include <stdlib.h>		
#include <glm/glm.hpp>		//camera y model
#include <glm/gtc/matrix_transform.hpp>		//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>		//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 200.0f, -300.0f));	//Posicion inicial de la camara
float MovementSpeed = 150.0f;	
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting		--Posicion de las luces
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f); //apunta de afuera del escenario, como si fuera un proyector
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);
glm::vec3 lightPosition2(1.0f, 1.0f, 5.0f);
glm::vec3 lightPosition3(220.0f, 1.0f, -6.0f);
glm::vec3 mueve(0.0f, 0.0f, 0.0f);

// posiciones
float	movAuto_x = 0.0f,
		movAuto_z = 0.0f,
		orienta = 0.0f;
bool	animacion = false,
		recorrido1 = true,
		recorrido2 = false,
		recorrido3 = false,
		recorrido4 = false;

float tiempo = 0.0f;
float cambio = 0.0f;
float cola = 0.0f;

float foco = 0.0f, x2=0.0f, limite=0.0f;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f;
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotInc = 0.0f,
		giroMonitoInc = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;


}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

void animate(void) //Anima a los objetos
{
	mueve.x = 300 * cos(tiempo);
	mueve.y += 20*cos(tiempo);
	tiempo += .3f;

	//if (cola  > 30.0f)
	//	cola -= 0.5f;
	//else
	//	cola += 0.5f;


	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += 0.1f;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces //Se cargan las texturas del fondo
	{
		"resources/skybox/Right_2.jpg",
		"resources/skybox/Left_2.jpg",
		"resources/skybox/Top_2.jpg",
		"resources/skybox/Bottom_2.jpg",
		"resources/skybox/Front_2.jpg",
		"resources/skybox/back_2.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
		//Model se usa para objetos estaticos, extension .obj recomendada
	Model base("modelos/build/build.obj");
	Model tib("modelos/Shark_/sharkBody-texture.obj");
	//Model shark("modelos/Shark/sharkBody-texture.obj");
	Model sharkTail("modelos/Shark_/sharkTail-texture.obj");
	//ModelAnim nemo("resources/nemo_anim/OBJ/Under The Sea.obj");
		//nemo.initShaders(animShader.ID);
	
	//Model torso("resources/objects/Personaje/torso.obj");
	//Model brazoDer("resources/objects/Personaje/brazoder.obj");
	//Model brazoIzq("resources/objects/Personaje/brazoizq.obj");
	//Model cabeza("resources/objects/Personaje/cabeza.obj");
	//Model carro("resources/objects/lambo/carroceria.obj");
	//Model llanta("resources/objects/lambo/Wheel.obj");
	//Model casaVieja("resources/objects/casa/OldHouse.obj");
	////Model cubo("resources/objects/cubo/cube02.obj");
	//Model casaDoll("resources/objects/casa/DollHouse.obj");

	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights ----- iluminación direccional (sol)
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));//Ilumina las caras iluminadas
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.3f, 0.3f, 0.3f)); //Ilumina las caras más ilumindas
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f)); //Reflejos de las superficies

		staticShader.setVec3("pointLight[0].position", lightPosition); //Iluminación posicional 
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.5f, 0.5f, 0.5f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f); //Distancia a la que iluminan los rayos 
		staticShader.setFloat("pointLight[0].linear", 0.09f); // ""
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);//potencia luz

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 3.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 1.0f, 0.0f));

		staticShader.setVec3("pointLight[1].position", lightPosition2);
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(cambio, 0.0f, cambio));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.09f);
		staticShader.setFloat("spotLight[0].quadratic", 0.05f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------

		//model = glm::translate(glm::mat4(1.0f), glm::vec3(40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		//model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//animShader.setMat4("model", model);
		//nemo.Draw(animShader);
		
		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 20.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		base.Draw(staticShader);

		tmp = model = glm::translate(model, glm::vec3(0.0f, 20.0f, 100.0f));
		//model = glm::scale(model, glm::vec3(.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		tib.Draw(staticShader);

		//------------------------ Arboles -------------------------------//
		//model = glm::translate(model, glm::vec3(100.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.5f));
		//staticShader.setMat4("model", model);
		//tree.Draw(staticShader);

		//model = glm::translate(tmp, glm::vec3(1000.0f, 30.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.5f));
		//staticShader.setMat4("model", model);
		//tree.Draw(staticShader);



		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje
		// -------------------------------------------------------------------------------------------------------------------------
		//TIBURON
		//model = glm::translate(glm::mat4(1.0f), glm::vec3(1000.0f, 0, 0));
		//tmp=model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//staticShader.setMat4("model", model);
		//shark.Draw(staticShader);
		
		////COLA TIBURON
		//model = glm::translate(tmp, glm::vec3(0.0f,1.5f, -30.0f));
		////model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(rotRodIzq), glm::vec3(0.0f, 1.0f, 0.0f));
		////model = glm::scale(model, glm::vec3(1.0f));
		//staticShader.setMat4("model", model);
		//sharkTail.Draw(staticShader);

		////Pierna Der
		//model = glm::translate(tmp, glm::vec3(-0.5f, 0.0f, -0.1f));
		//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		//staticShader.setMat4("model", model);
		//piernaDer.Draw(staticShader);

		////Pie Der
		//model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		//staticShader.setMat4("model", model);
		//botaDer.Draw(staticShader);

		////Pierna Izq
		//model = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
		//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//staticShader.setMat4("model", model);
		//piernaIzq.Draw(staticShader);

		////Pie Iz
		//model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		//staticShader.setMat4("model", model);
		//botaDer.Draw(staticShader);	//Izq trase

		////Brazo derecho
		//model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(-0.75f, 2.5f, 0));
		//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//staticShader.setMat4("model", model);
		//brazoDer.Draw(staticShader);

		////Brazo izquierdo
		//model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(0.75f, 2.5f, 0));
		//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//staticShader.setMat4("model", model);
		//brazoIzq.Draw(staticShader);

		////Cabeza
		//model = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0));
		//staticShader.setMat4("model", model);
		//cabeza.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------
		
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	//if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	//	posZ++;
	//if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	//	posZ--;
	//if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	//	posX--;
	//if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	//	posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		cambio+=.7;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		cambio -= .7;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		foco = 1;
	else
		foco = 0;
	//if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		//cambio -= .7;

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}