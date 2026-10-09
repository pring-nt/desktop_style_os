#include "shell/window_manager.h"

#include "imgui.h"

#include "apps/app_window.h"

namespace csopesy::shell {

namespace {

constexpr ImVec2 kFirstWindowOffset{48.0F, 40.0F};
constexpr ImVec2 kCascadeStep{32.0F, 32.0F};

}  // namespace

void WindowManager::Add(apps::AppWindow& window) {
  const auto index = static_cast<float>(windows_.size());
  window.set_default_offset(kFirstWindowOffset + (kCascadeStep * index));
  windows_.push_back(&window);
}

void WindowManager::ToggleFromTaskbar(apps::AppWindow& window) {
  if (IsActive(window)) {
    window.Minimize();
    active_ = nullptr;
    return;
  }
  window.Open();
  Activate(window);
}

void WindowManager::OpenAndActivate(apps::AppWindow& window) {
  window.Open();
  Activate(window);
}

void WindowManager::RenderAll(const apps::WorkArea& area) {
  for (apps::AppWindow* window : windows_) {
    window->Render(area);
    if (window->is_focused()) {
      Activate(*window);
    }
  }
}

bool WindowManager::IsActive(const apps::AppWindow& window) const {
  return active_ == &window && window.is_open() && !window.is_minimized();
}

void WindowManager::Activate(apps::AppWindow& window) { active_ = &window; }

}  // namespace csopesy::shell
