#include <print>
#include <thread>

auto heavyComputation() { std::println("Hello from thread!"); }

int main() {
  auto myThread = std::thread(heavyComputation);
  myThread.join();
}
// Listing 1.1: Simple thread creation