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

  ~App() { cancelAllOperations(); }

 private:
  auto startBackgroundThreads() -> void {
    for (unsigned i = 0; i < maxBackgroundThreads_; ++i) {
      backgroundThreads_.emplace_back(
          std::bind_front(&App::heavyComputation, this,
                          stopSource_.get_token()),
          i + 2);
    }
  }

  auto heavyComputation(std::stop_token token, int modulo)
      -> void {
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

  auto startUserInteraction() -> void {
    try {
      talkWithUser();
    } catch (const std::exception& e) {
      std::println("Exception occurred: {}", e.what());
      std::println("Returning safely");
    }
  }

  auto talkWithUser() -> void {
    std::println("Enter character to process (or 'q' to quit):");
    char input;
    while (not stopSource_.stop_requested()) {
      std::cin >> input;
      if (input == 'q') {
        cancelAllOperations();
        break;
      }
      if (input == 'e') {
        throw std::runtime_error("Simulated error");
      }
      std::println("You entered: {}", input);
    }
  }

  auto cancelAllOperations() noexcept -> void {
    stopSource_.request_stop();
  }

 private:
  unsigned maxBackgroundThreads_;
  std::vector<std::jthread> backgroundThreads_;
  std::stop_source stopSource_;
};

int main() {
  const auto maxThreads = getMaxNrOfBackgroundThreads();
  auto app = App{maxThreads};
  app.launch();
}
// Listing 1.10: Stop source to stop all threads simultaneously