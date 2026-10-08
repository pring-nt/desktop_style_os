// Entry point. Phase 1 bootstrap: opens a GLFW window with an OpenGL 3.3 core
// context and runs a Dear ImGui frame loop. Moves into the App class in
// Phase 2.

#include <cstdio>
#include <cstdlib>
#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "core/theme.h"

namespace {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr const char* kWindowTitle = "CSOPESY OS";
constexpr int kGlVersionMajor = 3;
constexpr int kGlVersionMinor = 3;
constexpr const char* kGlslVersion = "#version 330 core";
constexpr ImVec4 kClearColor{0.05F, 0.05F, 0.08F, 1.0F};

void PrintGlfwError(int code, const char* description) {
  std::fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

// Owns glfwInit() / glfwTerminate().
class GlfwSession {
 public:
  GlfwSession() : ok_(glfwInit() == GLFW_TRUE) {}
  ~GlfwSession() {
    if (ok_) {
      glfwTerminate();
    }
  }
  GlfwSession(const GlfwSession&) = delete;
  GlfwSession& operator=(const GlfwSession&) = delete;
  GlfwSession(GlfwSession&&) = delete;
  GlfwSession& operator=(GlfwSession&&) = delete;

  [[nodiscard]] bool ok() const { return ok_; }

 private:
  bool ok_;
};

struct WindowDeleter {
  void operator()(GLFWwindow* window) const { glfwDestroyWindow(window); }
};
using WindowPtr = std::unique_ptr<GLFWwindow, WindowDeleter>;

// Owns the ImGui context and its GLFW and OpenGL3 backends. Shuts down only
// the parts that initialized successfully.
class ImGuiSession {
 public:
  // Members initialize in declaration order, so the context exists before
  // either backend starts.
  explicit ImGuiSession(GLFWwindow* window)
      : context_ok_(CreateContext()),
        glfw_backend_ok_(ImGui_ImplGlfw_InitForOpenGL(window, true)),
        opengl_backend_ok_(glfw_backend_ok_ &&
                           ImGui_ImplOpenGL3_Init(kGlslVersion)) {}
  ~ImGuiSession() {
    if (opengl_backend_ok_) {
      ImGui_ImplOpenGL3_Shutdown();
    }
    if (glfw_backend_ok_) {
      ImGui_ImplGlfw_Shutdown();
    }
    ImGui::DestroyContext();
  }
  ImGuiSession(const ImGuiSession&) = delete;
  ImGuiSession& operator=(const ImGuiSession&) = delete;
  ImGuiSession(ImGuiSession&&) = delete;
  ImGuiSession& operator=(ImGuiSession&&) = delete;

  [[nodiscard]] bool ok() const { return opengl_backend_ok_; }

 private:
  static bool CreateContext() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    return true;
  }

  bool context_ok_;
  bool glfw_backend_ok_;
  bool opengl_backend_ok_;
};

void RenderFrame(GLFWwindow* window) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

#ifdef CSOPESY_SHOW_IMGUI_DEMO
  ImGui::ShowDemoWindow();
#endif

  ImGui::Render();
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glClearColor(kClearColor.x, kClearColor.y, kClearColor.z, kClearColor.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(window);
}

}  // namespace

int main() {
  glfwSetErrorCallback(PrintGlfwError);
  const GlfwSession glfw;
  if (!glfw.ok()) {
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kGlVersionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kGlVersionMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

  const WindowPtr window(glfwCreateWindow(kWindowWidth, kWindowHeight,
                                          kWindowTitle, nullptr, nullptr));
  if (!window) {
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window.get());
  glfwSwapInterval(1);

  if (gladLoadGL(glfwGetProcAddress) == 0) {
    std::fputs("Failed to load OpenGL functions\n", stderr);
    return EXIT_FAILURE;
  }

  const ImGuiSession imgui(window.get());
  if (!imgui.ok()) {
    return EXIT_FAILURE;
  }
  csopesy::core::Theme theme;
  theme.Apply();

  while (glfwWindowShouldClose(window.get()) == GLFW_FALSE) {
    glfwPollEvents();
    RenderFrame(window.get());
  }
  return EXIT_SUCCESS;
}
