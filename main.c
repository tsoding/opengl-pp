#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#endif // _WIN32
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Stolen from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/#draw-a-cube
static const GLfloat cube[][3] = {
    {-1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f,-1.0f},
    {1.0f,-1.0f,-1.0f},
    {1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {-1.0f,-1.0f, 1.0f},
    {1.0f,-1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f,-1.0f,-1.0f},
    {1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f,-1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f,-1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f,-1.0f},
    {-1.0f, 1.0f,-1.0f},
    {1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
    {1.0f,-1.0f, 1.0f}
};

const char * const scene_vertex_shader_source =
    "#version 130\n"
    "\n"
    "in vec3 position;\n"
    "uniform float angle;\n"
    "\n"
    "mat3 rotation = mat3(\n"
    "  cos(angle),  0.0f, sin(angle),\n"
    "  0.0f,        1.0f, 0.0f,\n"
    "  -sin(angle), 0.0f, cos(angle)\n"
    ");\n"
    "\n"
    "void main() {\n"
    "    gl_Position = vec4(position.xyz * 0.5f * rotation, 1.0f);\n"
    "}\n";
const char * const scene_fragment_shader_source =
    "#version 130\n"
    "\n"
    "void main() {\n"
    "    gl_FragColor = vec4(float(0x73) / 255.0f,\n"
    "                        float(0xc9) / 255.0f,\n"
    "                        float(0x36) / 255.0f,\n"
    "                        float(0xff) / 255.0f);\n"
    "}\n";

const char * const pp_vertex_shader_source =
    "#version 130\n"
    "out vec2 texcoord;"
    "void main(void)\n"
    "{\n"
    "    int gray = gl_VertexID ^ (gl_VertexID >> 1);\n"
    "    gl_Position = vec4(\n"
    "        2 * (gray / 2) - 1,\n"
    "        2 * (gray % 2) - 1,\n"
    "        0.0,\n"
    "        1.0);\n"
    "    texcoord = vec2(gray / 2, 1 - gray % 2);\n"
    "}\n";
const char * const pp_fragment_shader_source =
    "#version 130\n"
    "in vec2 texcoord;\n"
    "uniform sampler2D frame;\n"
    "vec2 direction = vec2(3.0, 3.0);\n"
    "vec2 iResolution = vec2(800, 600);\n"
    "out vec4 color;\n"
    "\n"
    "vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {\n"
    "  vec4 color = vec4(0.0);\n"
    "  vec2 off1 = vec2(1.411764705882353) * direction;\n"
    "  vec2 off2 = vec2(3.2941176470588234) * direction;\n"
    "  vec2 off3 = vec2(5.176470588235294) * direction;\n"
    "  color += texture2D(image, uv) * 0.1964825501511404;\n"
    "  color += texture2D(image, uv + (off1 / resolution)) * 0.2969069646728344;\n"
    "  color += texture2D(image, uv - (off1 / resolution)) * 0.2969069646728344;\n"
    "  color += texture2D(image, uv + (off2 / resolution)) * 0.09447039785044732;\n"
    "  color += texture2D(image, uv - (off2 / resolution)) * 0.09447039785044732;\n"
    "  color += texture2D(image, uv + (off3 / resolution)) * 0.010381362401148057;\n"
    "  color += texture2D(image, uv - (off3 / resolution)) * 0.010381362401148057;\n"
    "  return color;\n"
    "}\n"
    "\n"
    "vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {\n"
    "  vec4 color = vec4(0.0);\n"
    "  vec2 off1 = vec2(1.3846153846) * direction;\n"
    "  vec2 off2 = vec2(3.2307692308) * direction;\n"
    "  color += texture2D(image, uv) * 0.2270270270;\n"
    "  color += texture2D(image, uv + (off1 / resolution)) * 0.3162162162;\n"
    "  color += texture2D(image, uv - (off1 / resolution)) * 0.3162162162;\n"
    "  color += texture2D(image, uv + (off2 / resolution)) * 0.0702702703;\n"
    "  color += texture2D(image, uv - (off2 / resolution)) * 0.0702702703;\n"
    "  return color;\n"
    "}\n"
    "\n"
    "void main(void) {\n"
    "    vec2 uv = vec2(gl_FragCoord.xy / iResolution.xy);\n"
    "    color = blur13(frame, uv, iResolution.xy, direction);\n"
    "    //color = vec4(0.5, 0.5, 0.5, 1.0);\n"
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



int main(void)
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

#ifdef _WIN32
    if (GLEW_OK != glewInit()) {
        fprintf(stderr, "Could not initialize GLEW!\n");
    }
#endif // _WIN32

    GLuint scene_vertex_shader = compile_shader(scene_vertex_shader_source, GL_VERTEX_SHADER);
    GLuint scene_fragment_shader = compile_shader(scene_fragment_shader_source, GL_FRAGMENT_SHADER);
    GLuint scene_program = link_program(scene_vertex_shader, scene_fragment_shader);

    GLuint pp_vertex_shader = compile_shader(pp_vertex_shader_source, GL_VERTEX_SHADER);
    GLuint pp_fragment_shader = compile_shader(pp_fragment_shader_source, GL_FRAGMENT_SHADER);
    GLuint pp_program = link_program(pp_vertex_shader, pp_fragment_shader);


    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    const GLuint position_index = 0;
    glEnableVertexAttribArray(position_index);
    glVertexAttribPointer(position_index,
                          sizeof(cube[0]) / sizeof(cube[0][0]),
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          NULL);

    // FRAMEBUFFER SECTION BEGIN //////////////////////////////

    GLuint pp_framebuffer = 0;
    glGenFramebuffers(1, &pp_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, pp_framebuffer);

    GLuint pp_texture = 0;
    glGenTextures(1, &pp_texture);
    glBindTexture(GL_TEXTURE_2D, pp_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pp_texture, 0);
    // glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pp_texture, 0);
    GLenum draw_buffers = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &draw_buffers);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Mr. Buffer does not feel okay Sadge: %d\n", status);
        exit(1);
    }

    // FRAMEBUFFER SECTION END //////////////////////////////

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    GLint scene_angle_location = glGetUniformLocation(scene_program, "angle");
    float angle = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        // First pass
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(5.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, pp_framebuffer);
        glUseProgram(scene_program);
        glClear(GL_COLOR_BUFFER_BIT);
        angle = fmodf(angle + 0.01f, 2.0f * (float) M_PI);
        glUniform1f(scene_angle_location, angle);
        glDrawArrays(GL_TRIANGLES, position_index, sizeof(cube) / sizeof(cube[0]));

        // Second pass
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(pp_program);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_QUADS, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
