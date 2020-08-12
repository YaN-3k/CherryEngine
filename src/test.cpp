#include "CherryEngine.h"

/* test function to draw square */
int cherry::Draw() {
  if (glfwInit() == GLFW_FALSE) {
    std::cerr << "error: glfwInit()\n";
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Title", monitor, nullptr);

  if (window == nullptr) {
    std::cerr << "error: glfwCreateWindow()\n";
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cerr << "error: glewInit()\n";
    return EXIT_FAILURE;
  }

  constexpr auto vertex_shader_source = R"(
    #version 330 core
    layout (location = 0) in vec2 position;

    void main() {
      gl_Position = vec4(position, 0.f, 1.f);
    }
  )";

  constexpr auto fragment_shader_source = R"(
    #version 330 core
    out vec4 color;

    void main() {
      color = vec4(1.f, 0.5f, 0.2f, 1.0f);
    }
  )";

  auto CreateShader = [](auto shader_type, auto source) -> GLuint {
    auto shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
      GLint lenght;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &lenght);
      auto msg = static_cast<char *>(alloca(lenght));
      glGetShaderInfoLog(shader, lenght, &lenght, msg);
      std::cerr << "error: failed to compile "
                << (shader_type == GL_VERTEX_SHADER ? "veretex" : "fragment")
                << " shader";
      std::cerr << msg;
      return 0;
    }
    return shader;
  };

  auto CreateProgram = [](auto shader1, auto shader2) -> GLuint {
    auto program = glCreateProgram();
    glAttachShader(program, shader1);
    glAttachShader(program, shader2);
    glLinkProgram(program);
    glDeleteShader(shader1);
    glDeleteShader(shader2);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
      glDeleteProgram(program);
      GLint lenght;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &lenght);
      auto msg = static_cast<char *>(alloca(lenght));
      glGetProgramInfoLog(program, lenght, &lenght, msg);
      std::cerr << "error: failed to link program\n";
      std::cerr << msg;
      return 0;
    }
    return program;
  };

  GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_source);
  GLuint fragment_shader =
      CreateShader(GL_FRAGMENT_SHADER, fragment_shader_source);
  GLuint program = CreateProgram(vertex_shader, fragment_shader);

  GLfloat vertices[] = {
      0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,
  };

  GLuint indices[] = {0, 1, 3, 1, 2, 3};

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                        static_cast<void *>(0));
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  int width, height, channels;
  unsigned char *texture_data = stbi_load("container.jpg", &width, &height, &channels, 0);

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(program);
  glfwTerminate();
  return EXIT_SUCCESS;
}
