/*
Proyecto Final Computacion Grafica e interaccion Humano Computadora
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






Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];


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

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
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

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.0f, 0.3f);


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

	//CARGAR MODELOS
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


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/izquierda_dia.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up2.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	//unsigned int pointLightCount = 0;
	////Declaración de primer luz puntual
	//pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
	//	0.0f, 1.0f,
	//	2.0f, 1.5f, 1.5f,
	//	0.3f, 0.2f, 0.1f);
	//pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna asociada a la camara
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		8.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);


	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//luz ligada a la cámara de tipo flash 
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		


		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		//shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		glm::mat4 model(1.0);
		glm::mat4 auxiliar(1.0);
		glm::mat4 auxiliar2(1.0);

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
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 50.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Muros.RenderModel();
		//puerta Principal
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-121.0f, 0.0f, 138.0f));
		model = glm::scale(model, glm::vec3(9.5f, 12.8f, 15.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuertaP.RenderModel();
		//PUERTAS LIVERPOOL
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.0f, -0.5f, 73.5f));
		model = glm::scale(model, glm::vec3(9.5f, 18.5f, 25.5f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuertaI.RenderModel();
		
		//PUERTAS WALMART
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-79.0f, -0.5f, 43.1f));
		model = glm::scale(model, glm::vec3(9.5f, 18.5f, 25.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
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




		//MESA Y SILLAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-210.0f, 0.0f, -60.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MesaSilla.RenderModel();
		//MESA Y SILLAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-250.0f, 0.0f, -60.0f));
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

		//MODELO CARROUSEL
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-110.0f, 0.0f, -65.0f));
		model = glm::scale(model, glm::vec3(3.0f, 2.5f, 3.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		auxiliar2 = model;
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carrusel.RenderModel();
		//1
		model = glm::mat4(1.0);
		model = auxiliar2;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		/*model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));*/
		Caballo1.RenderModel();
		//2
		model = glm::mat4(1.0);
		model = auxiliar2;
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Caballo2.RenderModel();
		
		//3 rotar 180 en Y, -6.3Z
		model = glm::mat4(1.0);
		model = auxiliar2;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Caballo3.RenderModel();
		//4
		model = glm::mat4(1.0);
		model = auxiliar2;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Caballo4.RenderModel();




		//Renderizado AVATAR
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-180.0f, 0.0f, -110.0f));
		model = glm::scale(model, glm::vec3(4.5f, 4.5f, 4.5f));
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
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PIzquierdaAvatar.RenderModel();
		//PIERNA DERECHA
		model = glm::mat4(1.0);
		model = auxiliar;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PDerechaAvatar.RenderModel();
		
	
	


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
