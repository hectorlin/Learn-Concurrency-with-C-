#include <atomic>
#include <functional>
#include <iostream>
#include <print>
#include <stop_token>
#include <thread>
#include <vector>

auto getMaxNrOfBackgroundThreads() {
  const auto maxThreads = std::thread::hardware_concurrency();
  return maxThreads > 1 ? maxThreads - 1 : 1;
}

class App {
 public:
  explicit App(unsigned maxThreads)
      : maxBackgroundThreads_(maxThreads) {
    backgroundThreads_.reserve(maxThreads);
    std::println("Using {} threads for background computation",
                 maxBackgroundThreads_);
  }

  auto launch() -> void {
    startBackgroundThreads();
    startUserInteraction();
  }

  ~App() { cancelBackgroundOperations(); }

 private:
  auto startBackgroundThreads() -> void {
    for (unsigned i = 0; i < maxBackgroundThreads_; ++i) {
      backgroundThreads_.emplace_back(
          std::bind_front(&App::heavyComputation, this), i + 2);
    }
  }

  auto startUserInteraction() -> void {
    try {
      talkWithUser();
    } catch (const std::exception& e) {
      std::println("Exception occurred: {}", e.what());
      std::println("Returning safely");
    }
  }

  auto heavyComputation(std::stop_token token, int modulo) -> void {
    std::println("Simulating computation in background");
    auto sum = 0u;
    for (auto i = 0u; i < 1'000'000'000u; ++i) {
      sum += i % modulo;
      if (token.stop_requested()) {
        break;
      }
    }
    std::println("Computation finished. Result: {}", sum);
  }

  auto talkWithUser() -> void {
    std::println("Enter character to process (or 'q' to quit):");
    char input;
    while (std::cin >> input) {
      if (input == 'q') {
        cancelBackgroundOperations();
        break;
      }
      if (input == 'e') {
        throw std::runtime_error("Simulated error");
      }
      std::println("You entered: {}", input);
    }
  }

  auto cancelBackgroundOperations() noexcept -> void {
    for (auto& thread : backgroundThreads_) {
      thread.request_stop();
    }
  }

 private:
  unsigned maxBackgroundThreads_;
  std::vector<std::jthread> backgroundThreads_;
};

int main() {
  const auto maxThreads = getMaxNrOfBackgroundThreads();
  auto app = App{maxThreads};
  app.launch();
}
// Listing 1.8: Using jthread for background cancelation