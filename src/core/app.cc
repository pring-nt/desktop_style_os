#include "core/app.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "apps/app_window.h"
#include "boot/bios_screen.h"
#include "boot/splash_screen.h"
#include "core/paths.h"
#include "core/state_machine.h"
#include "core/theme.h"
#include "data/fun_facts.h"
#include "shell/desktop.h"
#include "shell/taskbar.h"

namespace csopesy::core {

namespace {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr const char* kWindowTitle = "CSOPESY OS";
constexpr int kGlVersionMajor = 3;
constexpr int kGlVersionMinor = 3;
constexpr const char* kGlslVersion = "#version 330 core";
constexpr ImVec4 kClearColor{0.0F, 0.0F, 0.0F, 1.0F};
constexpr const char* kWallpaperPath = "assets/frieren_wallpaper.jpg";

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

[[nodiscard]] WindowPtr CreateMainWindow() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kGlVersionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kGlVersionMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
  return WindowPtr(glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowTitle,
                                    nullptr, nullptr));
}

void BeginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void EndFrame(GLFWwindow* window) {
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

[[nodiscard]] bool AnyKeyOrClickPressed() {
  for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key) {
    if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key), false)) {
      return true;
    }
  }
  return false;
}

// The shutdown screen: one centered line of pixel text.
void DrawCenteredText(ImFont* font, const char* text) {
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  const float size = Theme::kBootFontSize * Theme::kBootTextScale;
  const ImVec2 text_size =
      font->CalcTextSizeA(size, viewport->WorkSize.x, 0.0F, text);
  const ImVec2 position =
      viewport->WorkPos + ((viewport->WorkSize - text_size) * 0.5F);
  ImGui::GetBackgroundDrawList()->AddText(
      font, size, position, ImGui::GetColorU32(ImGuiCol_Text), text);
}

[[nodiscard]] std::uint64_t TimeSeed() {
  return static_cast<std::uint64_t>(
      std::chrono::system_clock::now().time_since_epoch().count());
}

}  // namespace

App::App() : launch_seed_(TimeSeed()) {
  const auto add = [this](apps::AppWindow& window, shell::TaskbarIcon icon) {
    window_manager_.Add(window);
    taskbar_.Pin(window, icon);
  };
  add(file_explorer_, shell::TaskbarIcon::kFolder);
  add(terminal_, shell::TaskbarIcon::kTerminal);
  add(task_manager_, shell::TaskbarIcon::kActivity);
}

int App::Run() {
  glfwSetErrorCallback(PrintGlfwError);
  const GlfwSession glfw;
  if (!glfw.ok()) {
    return EXIT_FAILURE;
  }
  const WindowPtr window = CreateMainWindow();
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
  theme_.Apply(ExecutableDirectory() / Theme::kShellFontPath);
  desktop_.LoadWallpaper(ExecutableDirectory() / kWallpaperPath);
  fun_fact_ = data::PickFunFact(
      data::LoadFunFacts(ExecutableDirectory() / data::kFunFactsPath),
      launch_seed_);

  while (glfwWindowShouldClose(window.get()) == GLFW_FALSE) {
    glfwPollEvents();
    BeginFrame();
    Update(Seconds{ImGui::GetIO().DeltaTime});
    Render();
    if (state_machine_.should_exit()) {
      glfwSetWindowShouldClose(window.get(), GLFW_TRUE);
    }
    EndFrame(window.get());
  }
  // GL objects must be freed while the context still exists.
  desktop_.ReleaseWallpaper();
  return EXIT_SUCCESS;
}

void App::Update(Seconds elapsed) {
  if (state_machine_.state() == AppState::kBios && AnyKeyOrClickPressed()) {
    state_machine_.SkipBios();
  }
  state_machine_.Update(elapsed);
}

void App::Render() {
  switch (state_machine_.state()) {
    case AppState::kBios:
      boot::BiosScreen::Draw(state_machine_.time_in_state(), theme_.boot_font(),
                             fun_fact_);
      break;
    case AppState::kSplash:
      boot::SplashScreen::Draw(state_machine_.time_in_state(),
                               theme_.boot_font());
      break;
    case AppState::kDesktop:
      desktop_.Draw(clock_);
      window_manager_.RenderAll(
          shell::WorkAreaAboveTaskbar(shell::MainViewportRect()));
#ifdef CSOPESY_SHOW_IMGUI_DEMO
      ImGui::ShowDemoWindow();
#endif
      taskbar_.Draw(window_manager_, state_machine_);
      break;
    case AppState::kShutdown:
      DrawCenteredText(theme_.boot_font(), "Shutting down...");
      break;
  }
}

}  // namespace csopesy::core
