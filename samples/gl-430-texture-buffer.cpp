//**********************************
// OpenGL Texture Buffer
// 28/01/2013 - 28/01/2013
//**********************************
// Christophe Riccio
// ogl-samples@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include <glf/glf.hpp>

namespace
{
	char const * SAMPLE_NAME("OpenGL Buffer Texture");
	char const * VERTEX_SHADER_SOURCE("gl-430/texture-buffer.vert");
	char const * FRAGMENT_SHADER_SOURCE("gl-430/texture-buffer.frag");
	int const SAMPLE_SIZE_WIDTH(640);
	int const SAMPLE_SIZE_HEIGHT(480);
	int const SAMPLE_MAJOR_VERSION(4);
	int const SAMPLE_MINOR_VERSION(2);

	glf::window Window(glm::ivec2(SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT));

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const VertexData[VertexCount] = 
	{
		glm::vec2(-1.0f,-1.0f), 
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] = 
	{
		0, 1, 2, 
		2, 3, 0
	};

	enum buffer_type
	{
		BUFFER_VERTEX,
		BUFFER_ELEMENT,
		BUFFER_DISPLACEMENT,
		BUFFER_DIFFUSE,
		BUFFER_MAX
	};

	enum texture_type
	{
		TEXTURE_DISPLACEMENT,
		TEXTURE_DIFFUSE,
		TEXTURE_MAX
	};

	GLuint BufferName[BUFFER_MAX];
	GLuint TextureName[TEXTURE_MAX];
	GLuint VertexArrayName = 0;
	GLuint ProgramName = 0;
	GLint UniformMVP = 0;
	GLint UniformDiffuse = 0;
	GLint UniformDisplacement = 0;
	
}//namespace

bool initDebugOutput()
{
	bool Validated(true);

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageCallbackARB(&glf::debugOutput, NULL);

	return Validated;
}

bool initTest()
{
	bool Validated = true;
	glEnable(GL_DEPTH_TEST);

	return Validated && glf::checkError("initTest");
}

bool initProgram()
{
	bool Validated = true;
	
	// Create program
	if(Validated)
	{
		GLuint VertexShaderName = glf::createShader(GL_VERTEX_SHADER, glf::DATA_DIRECTORY + VERTEX_SHADER_SOURCE);
		GLuint FragmentShaderName = glf::createShader(GL_FRAGMENT_SHADER, glf::DATA_DIRECTORY + FRAGMENT_SHADER_SOURCE);

		ProgramName = glCreateProgram();
		glAttachShader(ProgramName, VertexShaderName);
		glAttachShader(ProgramName, FragmentShaderName);
		glDeleteShader(VertexShaderName);
		glDeleteShader(FragmentShaderName);
		glLinkProgram(ProgramName);
		Validated = glf::checkProgram(ProgramName);
	}

	// Get variables locations
	if(Validated)
	{
		UniformMVP = glGetUniformLocation(ProgramName, "MVP");
		UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
		UniformDisplacement = glGetUniformLocation(ProgramName, "Displacement");
	}

	return Validated;
}

bool initBuffer()
{
	glm::vec3 Displacement[5] = 
	{
		glm::vec3( 0.1f, 0.3f,-1.0f), 
		glm::vec3(-0.5f, 0.0f,-0.5f),
		glm::vec3(-0.2f,-0.2f, 0.0f),
		glm::vec3( 0.3f, 0.2f, 0.5f),
		glm::vec3( 0.1f,-0.3f, 1.0f)
	};

	glm::vec3 Diffuse[5] = 
	{
		glm::vec3(1.0f, 0.0f, 0.0f), 
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};	

	GLint TextureBufferOffsetAlignment(0);
	glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &TextureBufferOffsetAlignment);

	int DisplacementSize = sizeof(Displacement);
	int DiffuseSize = sizeof(Diffuse);
	int DisplacementMultiple = glm::higherMultiple(int(sizeof(Displacement)), int(TextureBufferOffsetAlignment));
	int DiffuseMultiple = glm::higherMultiple(int(sizeof(Diffuse)), int(TextureBufferOffsetAlignment));

	glGenBuffers(BUFFER_MAX, BufferName);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[BUFFER_ELEMENT]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, BufferName[BUFFER_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_TEXTURE_BUFFER, BufferName[BUFFER_DISPLACEMENT]);
	glBufferData(GL_TEXTURE_BUFFER, TextureBufferOffsetAlignment + DisplacementMultiple, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_TEXTURE_BUFFER, TextureBufferOffsetAlignment, sizeof(Displacement), Displacement);
/*
	void * PointerDisplacement = glMapBufferRange(GL_TEXTURE_BUFFER,
		0, sizeof(Displacement), GL_MAP_WRITE_BIT );
	memcpy(PointerDisplacement, &Displacement[0], sizeof(Displacement));
	glUnmapBuffer(GL_TEXTURE_BUFFER);
*/
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glBindBuffer(GL_TEXTURE_BUFFER, BufferName[BUFFER_DIFFUSE]);
	glBufferData(GL_TEXTURE_BUFFER, TextureBufferOffsetAlignment + DiffuseMultiple, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_TEXTURE_BUFFER, TextureBufferOffsetAlignment, sizeof(Diffuse), Diffuse);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	return true;
}

bool initTexture()
{
	GLint TextureBufferOffsetAlignment(0);
	glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &TextureBufferOffsetAlignment);

	glGenTextures(TEXTURE_MAX, TextureName);

	glBindTexture(GL_TEXTURE_BUFFER, TextureName[TEXTURE_DISPLACEMENT]);
	glTexBufferRange(GL_TEXTURE_BUFFER, GL_RGB32F, BufferName[BUFFER_DISPLACEMENT], TextureBufferOffsetAlignment, sizeof(glm::vec3) * 5);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	glBindTexture(GL_TEXTURE_BUFFER, TextureName[TEXTURE_DIFFUSE]);
	glTexBufferRange(GL_TEXTURE_BUFFER, GL_RGB32F, BufferName[BUFFER_DIFFUSE], TextureBufferOffsetAlignment, sizeof(glm::vec3) * 5);
	glBindTexture(GL_TEXTURE_BUFFER, 0);	

	return true;
}

bool initVertexArray()
{
	glGenVertexArrays(1, &VertexArrayName);
	glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName[BUFFER_VERTEX]);
		glVertexAttribPointer(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(glf::semantic::attr::POSITION);
	glBindVertexArray(0);

	return true;
}

bool begin()
{
	bool Validated(true);
	
	Validated = Validated && glf::checkGLVersion(SAMPLE_MAJOR_VERSION, SAMPLE_MINOR_VERSION);
	Validated = Validated && glf::checkExtension("GL_ARB_texture_buffer_range");

	if(Validated && glf::checkExtension("GL_ARB_debug_output"))
		Validated = initDebugOutput();
	if(Validated)
		Validated = initTest();
	if(Validated)
		Validated = initProgram();
	if(Validated)
		Validated = initBuffer();
	if(Validated)
		Validated = initTexture();
	if(Validated)
		Validated = initVertexArray();

	return Validated;
}

bool end()
{
	glDeleteTextures(TEXTURE_MAX, TextureName);
	glDeleteBuffers(BUFFER_MAX, BufferName);
	glDeleteProgram(ProgramName);
	glDeleteVertexArrays(1, &VertexArrayName);

	return true;
}

void display()
{
	// Compute the MVP (Model View Projection matrix)
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Window.TranlationCurrent.y));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Window.RotationCurrent.y, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 View = glm::rotate(ViewRotateX, Window.RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	glViewport(0, 0, Window.Size.x, Window.Size.y);

	float Depth(1.0f);
	glClearBufferfv(GL_DEPTH, 0, &Depth);
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);

	glUseProgram(ProgramName);
	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
	glUniform1i(UniformDisplacement, 0);
	glUniform1i(UniformDiffuse, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, TextureName[TEXTURE_DISPLACEMENT]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, TextureName[TEXTURE_DIFFUSE]);

	glBindVertexArray(VertexArrayName);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[BUFFER_ELEMENT]);
	glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, NULL, 5, 0);
	
	glf::swapBuffers();
}

int main(int argc, char* argv[])
{
	return glf::run(
		argc, argv,
		glm::ivec2(::SAMPLE_SIZE_WIDTH, ::SAMPLE_SIZE_HEIGHT), 
		GLF_CONTEXT_CORE_PROFILE_BIT, ::SAMPLE_MAJOR_VERSION, 
		::SAMPLE_MINOR_VERSION);
}
