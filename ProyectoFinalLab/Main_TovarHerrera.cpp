/*
Proyecto Final Computacion Grafica e interaccion Humano Computadora
TOVAR HERRERA CARLOS EDUARDO
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

//ARCHIVOS DE CABECERA
#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"


//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Camera cameraComida;
Camera cameraCarrusel;
Camera cameraAux;

Texture pisoTexture;


Model Muros;
Model PuertaP;
Model PuertaI;
Model PuertaD;

Model Carrusel;
Model Caballo1;
Model Caballo2;
Model Caballo3;
Model Caballo4;
Model MesaSilla;



Model TroncoAvatar;
Model BDerechoAvatar;
Model BIzquierdoAvatar;
Model PIzquierdaAvatar;
Model PDerechaAvatar;

Model mostrador1;
Model mostrador2;
Model puesto;

Skybox skybox;
Skybox skybox2;

//materiales
Material Material_brillante;
Material Material_opaco;

//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

SpotLight show[MAX_SPOT_LIGHTS];


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}




void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();

	CreateShaders();

	camera = Camera(glm::vec3(-50.0f, 0.0f, 180.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.0f, 0.3f);
	cameraComida = Camera(glm::vec3(-260.0f, 20.0f, -65.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.0f, 0.3f);
	cameraCarrusel = Camera(glm::vec3(-115.0f, 20.0f, 90.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.0f, 0.3f);



	//CARGAR TEXTURAS
	pisoTexture = Texture("Textures/piso_pr.tga");
	pisoTexture.LoadTextureA();
	//Modelo de plaza
	Muros = Model();
	Muros.LoadModel("Models/muros_2.obj");

	PuertaP = Model();
	PuertaP.LoadModel("Models/marco_puerta.obj");

	PuertaI = Model();
	PuertaI.LoadModel("Models/puerta_izquierda.obj");

	PuertaD = Model();
	PuertaD.LoadModel("Models/puerta_derecha.obj");

	//Modelo del carrusel
	Carrusel = Model();
	Carrusel.LoadModel("Models/carrusel_principal.obj");

	Caballo1 = Model();
	Caballo1.LoadModel("Models/caballo_1.obj");
	Caballo2 = Model();
	Caballo2.LoadModel("Models/caballo_2.obj");
	Caballo3 = Model();
	Caballo3.LoadModel("Models/caballo_3.obj");
	Caballo4 = Model();
	Caballo4.LoadModel("Models/caballo_4.obj");

	MesaSilla = Model();
	MesaSilla.LoadModel("Models/mesa_silla.obj");
	//Modelos para mostradores y puesto
	mostrador1 = Model();
	mostrador1.LoadModel("Models/mostradorWal.obj");
	mostrador2 = Model();
	mostrador2.LoadModel("Models/mostradorLiv.obj");
	puesto = Model();
	puesto.LoadModel("Models/puesto.obj");

	//CARGAR MODELOS VATAR
	TroncoAvatar = Model();
	TroncoAvatar.LoadModel("Models/tronco_avatar.obj");
	BDerechoAvatar = Model();
	BDerechoAvatar.LoadModel("Models/brazo_derecho_avatar.obj");
	BIzquierdoAvatar = Model();
	BIzquierdoAvatar.LoadModel("Models/brazo_izquierdo_avatar.obj");
	PIzquierdaAvatar = Model();
	PIzquierdaAvatar.LoadModel("Models/pierna_izquierda_avatar.obj");
	PDerechaAvatar = Model();
	PDerechaAvatar.LoadModel("Models/pierna_derecha_avatar.obj");


	//SKYBOX DE NOCHE
	std::vector<std::string> skyboxFacesN;
	skyboxFacesN.push_back("Textures/Skybox/izquierda_noche.tga");
	skyboxFacesN.push_back("Textures/Skybox/derecha_noche.tga");
	skyboxFacesN.push_back("Textures/Skybox/arriba_noche.tga");
	skyboxFacesN.push_back("Textures/Skybox/arriba_noche.tga");
	skyboxFacesN.push_back("Textures/Skybox/atras_noche.tga");
	skyboxFacesN.push_back("Textures/Skybox/frente_noche.tga");
	//SKYBOX DE DIA
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/izquierda_dia.tga");
	skyboxFaces.push_back("Textures/Skybox/derecha_dia.tga");
	skyboxFaces.push_back("Textures/Skybox/arriba_dia.tga");
	skyboxFaces.push_back("Textures/Skybox/arriba_dia.tga");
	skyboxFaces.push_back("Textures/Skybox/atras_dia.tga");
	skyboxFaces.push_back("Textures/Skybox/frente_dia.tga");


	skybox = Skybox(skyboxFaces);
	skybox2 = Skybox(skyboxFacesN);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	//LUCES PUNTUALES prenden en la noche
	unsigned int pointLightCount = 0;
	
	pointLights[0] = PointLight(0.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-250.0f, 2.0f, -100.0f,
		1.0f, 0.002f, 0.002f);
	pointLightCount++;

	pointLights[1] = PointLight(0.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-210.0f, 2.0f, -100.0f,
		1.0f, 0.002f, 0.002f);
	pointLightCount++;

	pointLights[2] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-138.0f, 2.0f, 147.0f,
		1.0f, 0.002f, 0.002f);
	pointLightCount++;

	pointLights[3] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-100.0f, 2.0f, 147.0f,
		1.0f, 0.002f, 0.002f);
	pointLightCount++;

	pointLights[4] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 2.0f, 147.0f,
		1.0f, 0.002f, 0.002f);
	pointLightCount++;

	//LUCES SPOTLIGHT luces de teclado 
	unsigned int spotLightCount = 0;
	
	spotLights[0] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 1.0f,
		-115.0f, 44.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		40.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		0.0f, 1.0f,
		-115.0f, 44.0f, 50.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		40.0f);
	spotLightCount++;

	spotLights[2] = SpotLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-115.0f, 30.0f, 100.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		40.0f);
	spotLightCount++;

	//LUCES SPOTLIGHT para show
	unsigned int spotLightCount2 = 0;
	spotLights[3] = SpotLight(0.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-115.0f, 30.0f, -60.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	spotLights[4] = SpotLight(1.0f, 1.0f, 0.0f,
		0.0f, 1.0f,
		-84.0f, 30.0f, -60.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount2++;

	spotLights[5] = SpotLight(1.0f, 0.0f, 1.0f,
		0.0f, 1.0f,
		-84.0f, 30.0f, -100.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	spotLights[6] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-115.0f, 30.0f, -100.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);

	float intervaloSkybox = 0.0; //CONTADOR PARA EL SKYBOX
	int dia = 1; // BANDERA PARA SABER SI ES DIA O NOCHE
	float showL = 0.0; 
	int luces = 0;
	int camara = 0;

	
	float posXavatar = 0.0f; //Controla el mov en X del avatar
	float posZavatar = 0.0f; //Controla el mov en Z del avatar
	float rotaAvatar = 0.0f; //Controla el rotacion del avatar
	int direccionAvatar = 0; //Bandera para la direccion de avatar

	float giroPuerta1 = 0.0f; //Bandera animacion puerta

	float muevePuertaP = 0.0f; //Variable para trasladar puerta principal
	float escalaXP = 0.0f;	//Variable para escalar puerta principal

	float rotaCarrusel = 0.0f; //Rotacion para carrusel y animales
	int giroCarrusel = 0;		//Bandera para rotacion
	int direccionCarrusel = 0; //Bandera mov de carrusel
	float posXcarrusel = 0.0f; //Mov de carrusel en X
	float posZcarrusel = 0.0f; //Mov de carrusel en Z

	float desplYcarrusel = 0.0f; //Sube y baja animales
	float offset = 0.0f;
	float desplYcarrusel2 = 0.0f; //Sube y baja animales
	float offset2 = 0.0f;


	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		if (mainWindow.camaraCom() == 0) {
			cameraComida.keyControl(mainWindow.getsKeys(), deltaTime);
			cameraComida.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else if (mainWindow.camaraCom() == 1) {
			cameraCarrusel.keyControl(mainWindow.getsKeys(), deltaTime);
			cameraCarrusel.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else {
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		
		
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//CAMBIO DE SKYBOX
		if (dia == 1) { // Comienza siendo dia
			if (intervaloSkybox < 300) { //Cuenta hasta llegar a 300
				if (mainWindow.camaraCom() == 0) {
					skybox.DrawSkybox(cameraComida.calculateViewMatrix(), projection);
				}
				else if (mainWindow.camaraCom() == 1) {
					skybox.DrawSkybox(cameraCarrusel.calculateViewMatrix(), projection);
				}
				else {
					skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
				}
				intervaloSkybox += 0.4 * deltaTime;
			}
			else {
				dia = 0; //Actualiza bandera y se cambia de noche
			}
		}
		if (dia == 0) { // Cambia a noche
			if (intervaloSkybox > 0) {
				if (mainWindow.camaraCom() == 0) {
					skybox2.DrawSkybox(cameraComida.calculateViewMatrix(), projection);
				}
				else if (mainWindow.camaraCom() == 1) {
					skybox2.DrawSkybox(cameraCarrusel.calculateViewMatrix(), projection);
				}
				else {
					skybox2.DrawSkybox(camera.calculateViewMatrix(), projection);
				}
				intervaloSkybox -= 0.4 * deltaTime;
			}
			else {
				dia = 1; //Actualiza bandera y se cambia a dia
			}
		}
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		
		if (mainWindow.camaraCom() == 0) {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(cameraComida.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, cameraComida.getCameraPosition().x, cameraComida.getCameraPosition().y, cameraComida.getCameraPosition().z);
		}
		else if (mainWindow.camaraCom() == 1) {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(cameraCarrusel.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, cameraCarrusel.getCameraPosition().x, cameraCarrusel.getCameraPosition().y, cameraCarrusel.getCameraPosition().z);
		}
		else {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}
		
		
		
		

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);

		
		//Luces que encienden de manera automatica cuando es de noche
		if (dia == 0) {
			shaderList[0].SetPointLights(pointLights, pointLightCount);//activa luz puntual
		}
		else {
			shaderList[0].SetPointLights(pointLights, 0);//desactiva luz puntual
		}

		//Luz por teclado
		if (mainWindow.luces() == true) {
			shaderList[0].SetSpotLights(spotLights, 1);//activa spotlight
			shaderList[0].SetSpotLights(spotLights, 2);//activa spotlight
			shaderList[0].SetSpotLights(spotLights, 3);//activa spotlight

		}
		else {
			shaderList[0].SetSpotLights(spotLights, 0);//desactiva luz puntual
		}

		//show de luces
		if (mainWindow.showLuces() == true && mainWindow.luces() == true) {
			if (luces == 0) {
				if (showL < 200) {
					showL += 0.5 * deltaTime;
					if (showL > 0) {
						shaderList[0].SetSpotLights(spotLights, 4);
					}
					if (showL > 50) {
						shaderList[0].SetSpotLights(spotLights, 5);
					}
					if (showL > 100) {
						shaderList[0].SetSpotLights(spotLights, 6);
					}
					if (showL > 150) {
						shaderList[0].SetSpotLights(spotLights, 7);
					}
				}
				else {
					luces = 1;
				}
			}
			if (luces == 1) {
				if (showL > 100) {
					showL -= 0.8 * deltaTime;
				}
				else {
					luces = 2;
				}
			}
			if (luces == 2) {
				if (showL < 200) {
					showL += 0.5 * deltaTime;
					if (showL > 100) {
						shaderList[0].SetSpotLights(spotLights, 4);
					}
					if (showL > 120) {
						shaderList[0].SetSpotLights(spotLights, 5);
					}
					if (showL > 140) {
						shaderList[0].SetSpotLights(spotLights, 6);
					}
					if (showL > 150) {
						shaderList[0].SetSpotLights(spotLights, 7);
					}

				}
				else {
					luces = 3;
				}
			}
			if (luces == 3) {
				if (showL > 0) {
					showL -= 0.8 * deltaTime;
					if (showL < 150 && showL > 100) {
						showL += 0.5 * deltaTime;
						shaderList[0].SetSpotLights(spotLights, 5);
					}
					if (showL < 100 && showL > 50) {
						showL += 0.5 * deltaTime;
						shaderList[0].SetSpotLights(spotLights, 6);
						shaderList[0].SetSpotLights(spotLights, 7);
					}
					
				}
				else {
					luces = 0;
				}
			}
		}

		glm::mat4 model(1.0);
		glm::mat4 auxiliar(1.0);
		glm::mat4 auxiliar2(1.0);
		glm::vec3 animacionAvatar(1.0);
		glm::vec3 animacionCarrusel(1.0);
		glm::vec3 escalarP1(1.0);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(40.0f, 1.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		//agregar material al plano de piso
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Modelo Muros Plaza
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 50.5f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Muros.RenderModel();
		//Animacion sencilla Puerta principal
		if (mainWindow.puertaPrin() == true) {
			if (muevePuertaP < 30 ) {
				muevePuertaP += 0.5 * deltaTime;
				if (escalaXP < 4) {
					escalaXP += 0.4 * deltaTime;
				}
			}
		}
		if (mainWindow.puertaPrin() == false) {
			if (muevePuertaP > 0 ) {
				muevePuertaP -= 0.5 * deltaTime;
				if (escalaXP > 0) {
					escalaXP -= 0.4 * deltaTime;
				}
			}
		}

		//puerta Principal
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-121.0f + muevePuertaP, -1.15f, 140.5f));
		model = glm::scale(model, glm::vec3(9.5f - escalaXP, 14.0f, 15.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuertaP.RenderModel();

		//Animacion sencilla puerta 1 
		if (mainWindow.puertaLiv() == true) {
			if (giroPuerta1 < 90) {
				giroPuerta1 += 0.5 * deltaTime;
			}
			else {
				escalarP1 = glm::vec3(9.5f, 0.0f, 25.5f);
			}
		}
		if (mainWindow.puertaLiv() == false) {
			if (giroPuerta1 > 0) {
				giroPuerta1 -= 0.5 * deltaTime;
			}
		}
		//PUERTAS LIVERPOOL
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.0f, -0.5f, 56.3f) );
		model = glm::scale(model, glm::vec3(9.5f, 18.0f, 24.8f) + escalarP1);
		model = glm::rotate(model, 0 + giroPuerta1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuertaI.RenderModel();
		
		//PUERTAS WALMART
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.0f, -0.5f, 44.0f));
		model = glm::scale(model, glm::vec3(9.5f, 18.5f, 25.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuertaI.RenderModel();

		//PUERTAS BAÑO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, -0.5f, -81.0f));
		model = glm::scale(model, glm::vec3(9.5f, 18.0f, 17.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuertaI.RenderModel();

		//PUERTAS BAÑO2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, -0.5f, -102.0f));
		model = glm::scale(model, glm::vec3(9.5f, 18.0f, 17.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuertaI.RenderModel();

		if (giroCarrusel == 0) {
			if (rotaCarrusel < 1000) {
				rotaCarrusel += 0.5 * deltaTime;
			}
			else {
				giroCarrusel = 1;
			}

		}
		if (giroCarrusel == 1) {
			if (rotaCarrusel > 0) {
				rotaCarrusel -= 0.5 * deltaTime;
			}
			else {
				giroCarrusel = 0;
			}
		}

		if (direccionCarrusel == 0)
		{
			if (posXcarrusel < 20.0)
			{
				posXcarrusel += 0.05 * deltaTime;
			}
			else {
				direccionCarrusel = 1;
			}
		}
		if (direccionCarrusel == 1)
		{
			if (posZcarrusel < 20.0)
			{
				posZcarrusel += 0.05 * deltaTime;
			}
			else {
				direccionCarrusel = 2;
			}
		}
		if (direccionCarrusel == 2)
		{
			if (posXcarrusel > 0.0)
			{
				posXcarrusel -= 0.05 * deltaTime;
			}
			else {
				direccionCarrusel = 3;
			}
		}
		if (direccionCarrusel == 3)
		{
			if (posZcarrusel > 0.0)
			{
				posZcarrusel -= 0.05 * deltaTime;
			}
			else {
				direccionCarrusel = 0;
			}
		}
	
		offset += 2.0 * deltaTime;
		desplYcarrusel = 0.3 * sin(offset * toRadians);

		offset2 += 2.0 * deltaTime;
		desplYcarrusel2 = 0.3 * cos(offset2 * toRadians);

		animacionCarrusel = glm::vec3(posXcarrusel, 0, posZcarrusel);
		
		//MODELO CARROUSEL
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-110.0f, 0.0f, -90.0f) + animacionCarrusel);
		model = glm::scale(model, glm::vec3(3.0f, 2.5f, 3.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		auxiliar2 = model;
		model = glm::rotate(model, 0 - rotaCarrusel * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carrusel.RenderModel();
		//1
		model = glm::mat4(1.0);
		model = auxiliar2;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f , 0.0f + desplYcarrusel));
		model = glm::rotate(model, 0 + rotaCarrusel * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Caballo1.RenderModel();

		//2
		model = glm::mat4(1.0);
		model = auxiliar2;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f + desplYcarrusel2));
		model = glm::rotate(model, 0 + rotaCarrusel * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Caballo2.RenderModel();

		////3 rotar
		model = glm::mat4(1.0);
		model = auxiliar2;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f + desplYcarrusel));
		model = glm::rotate(model, 0 + rotaCarrusel * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Caballo3.RenderModel();

		////4
		model = glm::mat4(1.0);
		model = auxiliar2;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f + desplYcarrusel2));
		model = glm::rotate(model, 0 + rotaCarrusel * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Caballo4.RenderModel();

		//MESA Y SILLAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-210.0f, 0.0f, -60.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MesaSilla.RenderModel();
		//MESA Y SILLAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-245.0f, 0.0f, -60.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MesaSilla.RenderModel();
		//MESA Y SILLAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-230.0f, 0.0f, -80.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MesaSilla.RenderModel();
		//MESA Y SILLAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-190.0f, 0.0f, -80.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MesaSilla.RenderModel();


		//ANIMACION AVATAR
		if (direccionAvatar == 0)
		{
			if (posZavatar < 53.0)
			{
				posZavatar += 0.05 * deltaTime;
			}
			else {
				do
				{
					rotaAvatar -= 0.1;
				} while (rotaAvatar >= -90);
				direccionAvatar = 1;
			}
		}
		if (direccionAvatar == 1)
		{
			if (posXavatar > -83.0)
			{
				posXavatar -= 0.05 * deltaTime;
			}
			else {
				do
				{
					rotaAvatar -= 0.1;
				} while (rotaAvatar >= -180);
				direccionAvatar = 2;
			}
		}
		if (direccionAvatar == 2)
		{
			if (posZavatar > 0.0)
			{
				posZavatar -= 0.05 * deltaTime;
			}
			else {
				do
				{
					rotaAvatar -= 0.1;
				} while (rotaAvatar >= -270);
				direccionAvatar = 3;
			}
		}
		if (direccionAvatar == 3)
		{
			if (posXavatar < 0.0)
			{
				posXavatar += 0.05 * deltaTime;
			}
			else {
				do
				{
					rotaAvatar += 0.1;
				} while (rotaAvatar <= 0);
				direccionAvatar = 0;
			}
		}
		animacionAvatar = glm::vec3(posXavatar, 0, posZavatar);

		//Renderizado AVATAR
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-180.0f, 0.0f, -100.0f) + animacionAvatar);
		model = glm::scale(model, glm::vec3(4.5f, 4.5f, 4.5f));
		model = glm::rotate(model, 0 + rotaAvatar * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		auxiliar = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		TroncoAvatar.RenderModel();
		//BRAZO DERECHO
		model = glm::mat4(1.0);
		model = auxiliar;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BDerechoAvatar.RenderModel(); 
		//BRAZO IZQUIERDO
		model = glm::mat4(1.0);
		model = auxiliar;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BIzquierdoAvatar.RenderModel();
		//PIERNA IZQUIERDA
		model = glm::mat4(1.0);
		model = auxiliar;
		//model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PIzquierdaAvatar.RenderModel();
		//PIERNA DERECHA
		model = glm::mat4(1.0);
		model = auxiliar;
		//model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PDerechaAvatar.RenderModel();

		//MODELO MOSTRADOR WALM
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 32.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mostrador1.RenderModel();
		//MODELO MOSTRADOR LIV
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 120.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mostrador2.RenderModel();
		//MODELO PUERTO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-140.0f, 0.0f, 50.0f));
		model = glm::scale(model, glm::vec3(4.5f, 8.5f, 5.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puesto.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
