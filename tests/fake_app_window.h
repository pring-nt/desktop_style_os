#ifndef CSOPESY_TESTS_FAKE_APP_WINDOW_H_
#define CSOPESY_TESTS_FAKE_APP_WINDOW_H_

#include <string>
#include <utility>

#include "imgui.h"

#include "apps/app_window.h"

namespace csopesy::testing {

// An app window that only counts how often its contents were drawn.
class FakeAppWindow : public apps::AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{400.0F, 300.0F};

  explicit FakeAppWindow(std::string title = "Fake")
      : AppWindow(std::move(title), kDefaultSize) {}

  [[nodiscard]] int draw_count() const { return draw_count_; }

 protected:
  void Draw() override { ++draw_count_; }

 private:
  int draw_count_ = 0;
};

}  // namespace csopesy::testing

#endif  // CSOPESY_TESTS_FAKE_APP_WINDOW_H_
