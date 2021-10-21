#include <array>
#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//Window pixels 
 int Width = 800;
 int Height = 600;

#pragma region Load/Debugger_Shaders 
std::string ReadFile(const char* FilePath) {

	std::string FileContents;
	if (std::ifstream FileStream{ FilePath, std::ios::in })
	{
		//Read file content 
		FileContents.assign(std::istreambuf_iterator<char>(FileStream), std::istreambuf_iterator<char>());
	}
	return FileContents;
}

void CheckShader(GLint ShaderId){

	GLint Result = GL_TRUE;
	glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &Result);

	if (Result == GL_FALSE)
	{
		GLint InfoLogLength = 0;
		glGetShaderiv(ShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0 )
		{
			std::string ShaderInfoLog(InfoLogLength, '/0');
			glGetShaderInfoLog(ShaderId, InfoLogLength, nullptr, &ShaderInfoLog[0]);

			std::cout << "Error in Shader" << std::endl;
			std::cout << ShaderInfoLog << std::endl;

			assert(false);
		}	
	}
}

GLuint LoadShaders(const char* VertexShaderFile, const char* FragmentShaderFile) {

	std::string VexterShaderSource = ReadFile(VertexShaderFile);
	std::string FragmentShaderSourse = ReadFile(FragmentShaderFile);

	assert(!VexterShaderSource.empty());
	assert(!FragmentShaderSourse.empty());

	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::cout << "Compilando" << VertexShaderFile << std::endl;
	const char* VexterShaderSoursePtr = VexterShaderSource.c_str();
	glShaderSource(VertexShaderID, 1, &VexterShaderSoursePtr, nullptr);
	glCompileShader(VertexShaderID);
	CheckShader(VertexShaderID);


	std::cout << "Compilando" << FragmentShaderFile << std::endl;
	const char* FragmentShaderSoursePtr = FragmentShaderSourse.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentShaderSoursePtr, nullptr);
	glCompileShader(FragmentShaderID);
	CheckShader(FragmentShaderID);


	std::cout << "Link Program" << std::endl;
	GLint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	GLint Result = GL_TRUE;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE)
	{
		GLint InfoLogLength = 0;
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0)
		{
			std::string ProgramInfoLog(InfoLogLength, '/0');
			glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramInfoLog[0]);

			std::cout << "Fail link program" << std::endl;
			std::cout << ProgramInfoLog << std::endl;

			assert(false);
		}		
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;

}

#pragma endregion

#pragma region Load Texture
GLuint LoadTexture(const char* TextureFile) {

	std::cout << "Loading Texture" << TextureFile << std::endl;

	stbi_set_flip_vertically_on_load(true);

	int TextureWidth = 0;
	int TextureHeight = 0;
	int NumberOfComponents = 0;
	unsigned char* TextureData = stbi_load(TextureFile, &TextureWidth, &TextureHeight, &NumberOfComponents, 3);
	assert(TextureData);

	GLuint TextureID;
	glGenTextures(1, &TextureID);

	glBindTexture(GL_TEXTURE_2D, TextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureWidth, TextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(TextureData);

	return TextureID;

}

#pragma endregion

struct Vertex {

	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
	glm::vec2 UV;

};

struct DirectionalLight {

	glm::vec3 Direction;
	GLfloat Intensity;
};

#pragma region GenerateMesh
GLuint LoadGeomtry() {

	std::array<Vertex, 6 > Quad = {
		Vertex{ glm::vec3{-1.0f,-1.0,0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec2{0.0f, 0.0f}},
		Vertex{ glm::vec3{1.0f,-1.0,0.0f},glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f},glm::vec2{1.0f, 0.0f}},
		Vertex{ glm::vec3{1.0f, 1.0, 0.0f},glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0.0f, 0.0f},glm::vec2{1.0f, 1.0f}},
		Vertex{ glm::vec3{-1.0f,1.0,0.0f},glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 1.0f}}
	};

	std::array<glm::ivec3, 2> Index = {
		glm::ivec3{ 0, 1, 3 },
		glm::ivec3{ 3, 1, 2 }
	};

	GLuint VertexBuffer;

	glGenBuffers(1, &VertexBuffer);

	GLuint ElementBuffer = 0;
	glGenBuffers(1, &ElementBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad), Quad.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index), Index.data(), GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*> (offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*> (offsetof(Vertex, Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*> (offsetof(Vertex, UV)));

	glBindVertexArray(0);

	return VAO;
}

void GenerateSphereMesh(GLuint Resolution, std::vector<Vertex>& Vertices, std::vector<glm::ivec3>& Indices) {

	Vertices.clear();
	Indices.clear();

	constexpr float Pi = glm::pi<float>();
	constexpr float TwoPi = glm::two_pi<float>();
	float InvResolution = 1.0f / static_cast<float>(Resolution - 1);

	for (GLuint UIndex = 0; UIndex < Resolution; ++UIndex)
	{
		const float U = UIndex * InvResolution;
		const float Phi = glm::mix(0.0f, TwoPi, U);

		for (GLuint VIndex = 0; VIndex < Resolution; ++VIndex)
		{
			const float V = VIndex * InvResolution;
			const float Theta = glm::mix(0.0f, Pi,V);

			glm::vec3 VertexPosition = {
				glm::sin(Theta) * glm::cos(Phi),
				glm::sin(Theta) * glm::sin(Phi),
				glm::cos(Theta)
			};

			Vertex Vertex{
				VertexPosition,
				glm::normalize(VertexPosition),
				glm::vec3{1.0f, 1.0f, 1.0f},
				glm::vec2{ U,V }
			};

			Vertices.push_back(Vertex);

		}
	}

	for (GLuint U = 0; U < Resolution - 1; U++)
	{
		for (GLuint V = 0; V < Resolution - 1; V++)
		{
			GLuint P0 = U + V * Resolution;
			GLuint P1 = (U + 1) + V * Resolution;
			GLuint P2 = (U + 1) + (V + 1) * Resolution;
			GLuint P3 = U + (V + 1) * Resolution;

			Indices.push_back(glm::ivec3{ P0, P1, P3 });
			Indices.push_back(glm::ivec3{ P3,P1, P2 });
		}
	}
}

GLuint LoadSphere(GLuint& NumVertex, GLuint& NumIndices) {

	std::vector<Vertex> Vertices;
	std::vector<glm::ivec3>Triangles;
	GenerateSphereMesh(100, Vertices, Triangles);

	NumVertex = Vertices.size();
	NumIndices = Triangles.size() * 3;

	GLuint VertexBuffer;
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

	GLuint ElementBuffer;
	glGenBuffers(1, &ElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumIndices * sizeof(GLuint), Triangles.data(), GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*> (offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*> (offsetof(Vertex, Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*> (offsetof(Vertex, UV)));

	glBindVertexArray(0);

	return VAO;
}
#pragma endregion

//Camera Behaviour-------------------//
class FlyCamera {
public:
	float Speed = 1.0f;
	float Sensitivity = 0.01f;

	void MoveFoward(float Amount) {
		Location += glm::normalize(Direction) * Amount * Speed;
	}
	void MoveRight(float Amount) {
		glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));
		Location += Right * Amount * Speed;
	}

	void Look(float Yaw, float Pitch) {

		Yaw *= Sensitivity;
		Pitch *= Sensitivity;

		const glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));

		const glm::mat4 I = glm::identity<glm::mat4>();
		glm::mat4 YawRotation = glm::rotate(I, glm::radians(Yaw),Up);
		glm::mat4 PitchRotation = glm::rotate(I, glm::radians(Pitch), Right);

		Up = PitchRotation * glm::vec4{ Up, 0.0f };
		Direction = YawRotation * PitchRotation * glm::vec4{ Direction, 0.0f };
	}

	glm::mat4 GetView() const{
		return glm::lookAt(Location, Location + Direction, Up);
	}

	glm::mat4 GetViewProjection() const {

		glm::mat4 Projection = glm::perspective(FoV, AspectRatio, Near, Far);
		return Projection * GetView();
	}

	//View Matrix
	glm::vec3 Location{ 0.0f, 0.0f, 10.0f };
	glm::vec3 Direction{ 0.0f, 0.0f, -1.0f };
	glm::vec3 Up{ 0.0f, 1.0f, 0.0f };

	//Projection Matrix
	float FoV = glm::radians(45.0f);
	float AspectRatio = Width / Height;
	float Near = 0.01f;
	float Far = 1000.0f;

};

FlyCamera Camera;
bool bEnableMouseMovement = false;
glm::vec2 PreviousCursor{ 0.0, 0.0 };
//--------------------------------//

#pragma region CallBacks
//Button input Mouse 
void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Modifiers) {

	if (Button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (Action == GLFW_PRESS)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			double x, y;
			glfwGetCursorPos(Window, &x, &y);

			PreviousCursor = glm::vec2{ x, y };
			bEnableMouseMovement = true;
		}
		if (Action == GLFW_RELEASE)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			bEnableMouseMovement = false;
		}
	}
}

//Movement Mouse 
void MouseMotionCallback(GLFWwindow* Window, double x, double y) {

	if (bEnableMouseMovement)
	{
		glm::vec2 CurrentCursor{ x,y };
		glm::vec2 DeltaCursor = CurrentCursor - PreviousCursor;

		Camera.Look(DeltaCursor.x, DeltaCursor.y);

		PreviousCursor = CurrentCursor;
	}

}

//Update Geometry with scale of screen
void Resize(GLFWwindow* Window, int NewWidth, int NewHeigth) {

	Width = NewWidth;
	Height = NewHeigth;
	Camera.AspectRatio = static_cast<float>(Width) / Height;

	glViewport(0, 0, Width, Height);
}
#pragma endregion


int main() {

	//Initialize Library
 	assert(glfwInit() == GLFW_TRUE);

	//Create Window
	GLFWwindow* window = glfwCreateWindow(Width, Height, "BlueMarbel", nullptr, nullptr);
	assert(window);

	//CallBacks
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, MouseMotionCallback);
	glfwSetFramebufferSizeCallback(window, Resize);

	//Ativa o contexto criado na janela window
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//Initialize Library GLEW
	assert(glewInit() == GLEW_OK);

#pragma region Debbuger/Load

	//Verify Version OpenGL --------- ("Verify version OpenGL is active")
	GLint GLMajorVersion = 0;
	GLint GLMinorVersion = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &GLMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &GLMinorVersion);
	std::cout << "OpenGL Version" << GLMajorVersion << "." << GLMinorVersion << std::endl;

	//Use glGetString to now drivers version 
	std::cout << "OpenGL Vendor : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL Renderer : " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLGS Version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	Resize(window, Width, Height);

	GLuint ProgramID = LoadShaders("shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");
	GLuint TextureID = LoadTexture("textures/earth_2k.jpg");
	GLuint CloudTextureId = LoadTexture("textures/earth_clouds_2k.jpg");

#pragma endregion

#pragma region Initialize OpenGl
	GLuint SphereNumVertices = 0;
	GLuint SphereNumIndices = 0;
	GLuint SphereVAO = LoadSphere(SphereNumVertices, SphereNumIndices);
	
	//Model Matrix
	glm::mat4 I = glm::identity <glm::mat4>();
	glm::mat4 ModelMatrix = glm::rotate(I, glm::radians(90.0f), glm::vec3{ 1,0,0 });

	//Define Color BG
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	double PreviousTime = glfwGetTime();

	//backface Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	//z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	DirectionalLight Light;
	Light.Direction = glm::vec3{ 0.0f,0.0f,1.0f };
	Light.Intensity = 1.2f;

#pragma endregion

	//Loop Events
	while (!glfwWindowShouldClose(window))
	{
		//Delta Time 
		double CurrentTime = glfwGetTime();
		double DeltaTime = CurrentTime - PreviousTime;
		if (DeltaTime > 0.0)
		{
			PreviousTime = CurrentTime;
		}

		//Clean framebuffer 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(ProgramID);

		glm::mat4 NormalMatrix = glm::inverse(glm::transpose(Camera.GetView() * ModelMatrix));
		glm::mat4 ViewProjectionMatrix = Camera.GetViewProjection();
		glm::mat4 ModelViewProjection = ViewProjectionMatrix * ModelMatrix;

		GLint TimeLoc = glGetUniformLocation(ProgramID, "Time");
		glUniform1f(TimeLoc, CurrentTime);

		GLint NormalMatrixLoc = glGetUniformLocation(ProgramID, "NormalMatrix");
		glUniformMatrix4fv(NormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

		GLint ModelViewProjectionLoc = glGetUniformLocation(ProgramID, "MVP");
		glUniformMatrix4fv(ModelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, CloudTextureId);

		GLint TextureSamplerLoc = glGetUniformLocation(ProgramID, "Texture");
		glUniform1i(TextureSamplerLoc, 0);

		GLint CloudTextureLoc = glGetUniformLocation(ProgramID, "CloudsTexture");
		glUniform1i(CloudTextureLoc, 1);

		GLint LightDirectionLoc = glGetUniformLocation(ProgramID, "LightDirection");
		glUniform3fv(LightDirectionLoc, 1, glm::value_ptr(Camera.GetView() * glm::vec4{ Light.Direction ,0.0f}));

		GLint LightIntensityLoc = glGetUniformLocation(ProgramID, "LightIntensity");
		glUniform1f(LightIntensityLoc,Light.Intensity);

		glBindVertexArray(SphereVAO);
		glLineWidth(10.0f);
		glPointSize(10.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glDrawElements(GL_TRIANGLES, SphereNumIndices, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);

		glUseProgram(0);

		//Process all commands in line 
		glfwPollEvents();

		//Send the info for frameBuffer draw on window
		glfwSwapBuffers(window);

      #pragma region Input
		if (glfwGetKey(window, GLFW_KEY_W)== GLFW_PRESS)
		{
			Camera.MoveFoward(1.0f * DeltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Camera.MoveFoward(-1.0f * DeltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Camera.MoveRight(-1.0f * DeltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Camera.MoveRight(1.0f * DeltaTime);
		}
	}
      #pragma endregion

	// Deslocated Vertex Buffer
	glDeleteVertexArrays(1, &SphereVAO);

	//Finish Library
	glfwTerminate();

	return 0;
}