//**********************************
// OpenGL Memory Barrier
// 11/03/2011 - 13/08/2011
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
	std::string const SAMPLE_NAME = "OpenGL Memory barrier";
	std::string const SHADER_VERT_SOURCE_UPDATE(glf::DATA_DIRECTORY + "420/memory-barrier-update.vert");
	std::string const SHADER_FRAG_SOURCE_UPDATE(glf::DATA_DIRECTORY + "420/memory-barrier-update.frag");
	std::string const SHADER_VERT_SOURCE_BLIT(glf::DATA_DIRECTORY + "420/memory-barrier-blit.vert");
	std::string const SHADER_FRAG_SOURCE_BLIT(glf::DATA_DIRECTORY + "420/memory-barrier-blit.frag");
	std::string const TEXTURE_DIFFUSE(glf::DATA_DIRECTORY + "kueken640-rgb8.dds");
	glm::ivec2 FRAMEBUFFER_SIZE(0);
	int const SAMPLE_SIZE_WIDTH(640);
	int const SAMPLE_SIZE_HEIGHT(480);
	int const SAMPLE_MAJOR_VERSION(4);
	int const SAMPLE_MINOR_VERSION(1);

	glf::window Window(glm::ivec2(SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT));

	GLsizei const VertexCount(6);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 0.0f))
	};

	namespace program
	{
		enum type
		{
			UPDATE,
			BLIT,
			MAX
		};
	}//namespace program
	
	namespace pipeline
	{
		enum type
		{
			UPDATE,
			BLIT,
			MAX
		};
	}//namespace pipeline

	namespace buffer
	{
		enum type
		{
			VERTEX,
			TRANSFORM,
			MAX
		};
	}//namespace buffer

	namespace texture
	{
		enum type
		{
			DIFFUSE,
			COLORBUFFER,
			MAX
		};
	}//namespace texture

	GLuint VertexArrayName(0);
	GLuint BufferName[buffer::MAX] = {0, 0};
	GLuint TextureName[texture::MAX] = {0, 0};
	GLuint SamplerName(0);
	GLuint FramebufferName(0);

	GLuint ProgramName[program::MAX];
	GLuint PipelineName[pipeline::MAX];
}//namespace

bool initProgram()
{
	bool Validated(true);
	
	glGenProgramPipelines(pipeline::MAX, PipelineName);
	glBindProgramPipeline(PipelineName[pipeline::UPDATE]);
	glBindProgramPipeline(PipelineName[pipeline::BLIT]);
	glBindProgramPipeline(0);

	if(Validated)
	{
		GLuint VertShaderName = glf::createShader(GL_VERTEX_SHADER, SHADER_VERT_SOURCE_UPDATE);
		GLuint FragShaderName = glf::createShader(GL_FRAGMENT_SHADER, SHADER_FRAG_SOURCE_UPDATE);

		ProgramName[program::UPDATE] = glCreateProgram();
		glProgramParameteri(ProgramName[program::UPDATE], GL_PROGRAM_SEPARABLE, GL_TRUE);
		glAttachShader(ProgramName[program::UPDATE], VertShaderName);
		glAttachShader(ProgramName[program::UPDATE], FragShaderName);
		glDeleteShader(VertShaderName);
		glDeleteShader(FragShaderName);
		glLinkProgram(ProgramName[program::UPDATE]);
		Validated = Validated && glf::checkProgram(ProgramName[program::UPDATE]);
	}

	if(Validated)
	{
		glUseProgramStages(PipelineName[pipeline::UPDATE], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::UPDATE]);
		Validated = Validated && glf::checkError("initProgram - stage");
	}

	if(Validated)
	{
		GLuint VertShaderName = glf::createShader(GL_VERTEX_SHADER, SHADER_VERT_SOURCE_BLIT);
		GLuint FragShaderName = glf::createShader(GL_FRAGMENT_SHADER, SHADER_FRAG_SOURCE_BLIT);

		ProgramName[program::BLIT] = glCreateProgram();
		glProgramParameteri(ProgramName[program::BLIT], GL_PROGRAM_SEPARABLE, GL_TRUE);
		glAttachShader(ProgramName[program::BLIT], VertShaderName);
		glAttachShader(ProgramName[program::BLIT], FragShaderName);
		glDeleteShader(VertShaderName);
		glDeleteShader(FragShaderName);
		glLinkProgram(ProgramName[program::BLIT]);
		Validated = Validated && glf::checkProgram(ProgramName[program::BLIT]);
	}

	if(Validated)
	{
		glUseProgramStages(PipelineName[pipeline::BLIT], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::BLIT]);
		Validated = Validated && glf::checkError("initProgram - stage");
	}

	return Validated;
}

bool initArrayBuffer()
{
	bool Validated(true);

	glGenBuffers(1, &BufferName[buffer::VERTEX]);
    glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return Validated;
}

bool initSampler()
{
	bool Validated(true);

	glGenSamplers(1, &SamplerName);
	glSamplerParameteri(SamplerName, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(SamplerName, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return Validated;
}

bool initTexture2D()
{
	bool Validated(true);

	gli::texture2D Image = gli::load(TEXTURE_DIFFUSE);
	FRAMEBUFFER_SIZE = Image[0].dimensions();

	glActiveTexture(GL_TEXTURE0);
	if(!TextureName[texture::DIFFUSE])
		glGenTextures(texture::MAX, TextureName);

	glBindTexture(GL_TEXTURE_2D, TextureName[texture::DIFFUSE]);
	for(std::size_t Level = 0; Level < Image.levels(); ++Level)
	{
		glTexImage2D(
			GL_TEXTURE_2D, 
			GLint(Level), 
			GL_RGBA8, 
			GLsizei(Image[Level].dimensions().x), 
			GLsizei(Image[Level].dimensions().y), 
			0,  
			GL_BGR, 
			GL_UNSIGNED_BYTE, 
			Image[Level].data());
	}

	glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y);

	return Validated;
}

bool initFramebuffer()
{
	bool Validated(true);

	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[texture::COLORBUFFER], 0);

	Validated = Validated && (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return Validated;
}

bool initVertexArray()
{
	bool Validated(true);

	glGenVertexArrays(1, &VertexArrayName);
    glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glVertexAttribPointer(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), GLF_BUFFER_OFFSET(0));
		glVertexAttribPointer(glf::semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), GLF_BUFFER_OFFSET(sizeof(glm::vec2)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(glf::semantic::attr::POSITION);
		glEnableVertexAttribArray(glf::semantic::attr::TEXCOORD);
	glBindVertexArray(0);

	return Validated;
}

bool initUniformBuffer()
{
	bool Validated(true);

	GLint UniformBufferOffset(0);

	glGetIntegerv(
		GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
		&UniformBufferOffset);

	GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

	glGenBuffers(1, &BufferName[buffer::TRANSFORM]);
	glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
	glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return Validated;
}

bool initDebugOutput()
{
	bool Validated(true);

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageCallbackARB(&glf::debugOutput, NULL);

	return Validated;
}

bool begin()
{
	bool Validated(true);
	Validated = Validated && glf::checkGLVersion(SAMPLE_MAJOR_VERSION, SAMPLE_MINOR_VERSION);

	if(Validated && glf::checkExtension("GL_ARB_debug_output"))
		Validated = initDebugOutput();
	if(Validated)
		Validated = initProgram();
	if(Validated)
		Validated = initArrayBuffer();
	if(Validated)
		Validated = initVertexArray();
	if(Validated)
		Validated = initTexture2D();
	if(Validated)
		Validated = initSampler();
	if(Validated)
		Validated = initFramebuffer();
	if(Validated)
		Validated = initUniformBuffer();

	return Validated;
}

bool end()
{
	bool Validated(true);

	glDeleteTextures(texture::MAX, TextureName);
	glDeleteFramebuffers(1, &FramebufferName);
	glDeleteBuffers(buffer::MAX, BufferName);
	glDeleteProgram(ProgramName[program::BLIT]);
	glDeleteProgram(ProgramName[program::UPDATE]);
	glDeleteVertexArrays(1, &VertexArrayName);
	glDeleteSamplers(1, &SamplerName);
	glDeleteProgramPipelines(pipeline::MAX, PipelineName);

	return Validated;
}

void display()
{
	static int FrameIndex = 0;

	{
		glm::mat4 MVP = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(MVP), GL_MAP_WRITE_BIT);
		*Pointer = MVP;
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

	// Bind shared objects
	glViewportIndexedf(0, 0, 0, float(Window.Size.x), float(Window.Size.y));
	glBindBufferBase(GL_UNIFORM_BUFFER, glf::semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
	glBindSampler(0, SamplerName);
	glBindVertexArray(VertexArrayName);

	// Update a colorbuffer bound as a framebuffer attachement and as a texture
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glBindProgramPipeline(PipelineName[pipeline::UPDATE]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FrameIndex ? TextureName[texture::COLORBUFFER] : TextureName[texture::DIFFUSE]);

	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
	glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);

	// Blit to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindProgramPipeline(PipelineName[pipeline::BLIT]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);

	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
	glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
	glf::swapBuffers();

	FrameIndex = (FrameIndex + 1) % 256;
}

int main(int argc, char* argv[])
{
	return glf::run(
		argc, argv,
		glm::ivec2(::SAMPLE_SIZE_WIDTH, ::SAMPLE_SIZE_HEIGHT), 
		::SAMPLE_MAJOR_VERSION, 
		::SAMPLE_MINOR_VERSION);
}
