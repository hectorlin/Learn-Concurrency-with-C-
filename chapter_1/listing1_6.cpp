#include <iostream>
#include <print>
#include <thread>

auto heavyComputation() {
  std::println("Simulating computation in background");
  auto sum = 0u;
  for (auto i = 0u; i < 1'000'000'000u; ++i) {
    sum += i % 2;
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
  auto backgroundThread = std::thread(heavyComputation);
  talkWithUser();
  backgroundThread.join();
}

int main() {
  try {
    launchApp();
  } catch (const std::exception& e) {
    std::println("Exception occurred: {}", e.what());
    std::println("Returning safely");
  }
}
// Listing 1.6: Unsafe exception handling