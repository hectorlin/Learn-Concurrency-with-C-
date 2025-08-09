#include <iostream>
#include <print>
#include <thread>
#include <vector>

auto getMaxNrOfBackgroundThreads() {
  const auto maxThreads = std::thread::hardware_concurrency();
  return maxThreads > 1 ? maxThreads - 1 : 1;
}

auto heavyComputation(int modulo) {
  std::println("Simulating computation in background");
  auto sum = 0u;
  for (auto i = 0u; i < 1'000'000'000u; ++i) {
    sum += i % modulo;
  }
  std::println("Computation finished. Result: {}", sum);
}

auto talkWithUser() {
  std::println("Enter character to process (or 'q' to quit):");
  char input;
  while (std::cin >> input) {
    if (input == 'q') {
      break;
    }
    if (input == 'e') {
      throw std::runtime_error("Simulated error");
    }
    std::println("You entered: {}", input);
  }
}

auto launchApp() {
  const auto maxBackgroundThreads = getMaxNrOfBackgroundThreads();
  std::println("Using {} threads for background computation",
               maxBackgroundThreads);

  auto backgroundThreads = std::vector<std::jthread>{};
  backgroundThreads.reserve(maxBackgroundThreads);
  for (int i = 0; i < maxBackgroundThreads; i++) {
    backgroundThreads.emplace_back(heavyComputation, i + 2);
  }

  try {
    talkWithUser();
  } catch (const std::exception& e) {
    std::println("Exception occurred: {}", e.what());
    std::println("Returning safely");
  }
}

int main() { launchApp(); }
// Listing 1.6: Background computation with jthreads