#include <print>
#include <thread>

int main() {
  auto myJoiningThread = std::jthread(
      [](int val) {
        std::println("Hello from thread! Value: {}", val);
      },
      21);
}
// Listing 1.3: JThread example