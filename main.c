#include <stdio.h>
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

float quad[4][2] = {
    {-0.5f, -0.5f},
    {-0.5f,  0.5f},
    { 0.5f,  0.5f},
    { 0.5f, -0.5f},
};

const char * const vertex_shader_source =
    "#version 130\n"
    "\n"
    "in vec2 position;\n"
    "\n"
    "void main() {\n"
    "    gl_Position = vec4(position.xy * 0.5f, 0.0f, 1.0f);\n"
    "}\n";
const char * const fragment_shader_source =
    "#version 130\n"
    "\n"
    "void main() {\n"
    "    gl_FragColor = vec4(float(0x73) / 255.0f,\n"
    "                        float(0xc9) / 255.0f,\n"
    "                        float(0x36) / 255.0f,\n"
    "                        float(0xff) / 255.0f);\n"
    "}\n";

const char *shader_type_as_cstr(GLenum shader_type)
{
    switch (shader_type) {
    case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
    case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
    case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
    default:                 return "<unknown>";
    }
}

GLuint compile_shader(const char *source_code, GLenum shader_type)
{
    GLuint shader = 0;
    shader = glCreateShader(shader_type);
    if (shader == 0) {
        fprintf(stderr, "Could not create a shader %s",
                shader_type_as_cstr(shader_type));
        exit(1);
    }

    glShaderSource(shader, 1, &source_code, 0);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLchar buffer[1024];
        int length = 0;
        glGetShaderInfoLog(shader, sizeof(buffer), &length, buffer);
        fprintf(stderr, "Could not compile shader %s: %s\n",
                shader_type_as_cstr(shader_type), buffer);
        exit(1);
    }

    return shader;
}

GLuint link_program(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();

    if (program == 0) {
        fprintf(stderr, "Could not create shader program");
        exit(1);
    }

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLchar buffer[1024];
        int length = 0;
        glGetProgramInfoLog(program, sizeof(buffer), &length, buffer);
        fprintf(stderr, "Could not link the program: %s\n", buffer);
        exit(1);
    }

    return program;
}

int main(int argc, char *argv[])
{
    if (!glfwInit()) {
        fprintf(stderr, "Could not initialize GLFW!\n");
        exit(1);
    }

    GLFWwindow *window = glfwCreateWindow(
        SCREEN_WIDTH, SCREEN_HEIGHT,
        "OpenGL Post-Processing", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        fprintf(stderr, "Could not create GLFW window!\n");
        exit(1);
    }

    glfwMakeContextCurrent(window);

    GLuint vertex_shader = compile_shader(vertex_shader_source, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_shader_source, GL_FRAGMENT_SHADER);
    GLuint program = link_program(vertex_shader, fragment_shader);

    glUseProgram(program);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    const GLuint position_index = 0;
    glEnableVertexAttribArray(position_index);
    glVertexAttribPointer(position_index,
                          sizeof(quad[0]) / sizeof(quad[0][0]),
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          NULL);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_QUADS, position_index, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
