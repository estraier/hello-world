#include <chrono>
#include <string>
#include <cstdlib>
#include <cassert>
#include <iomanip>
#include <iostream>

constexpr int num_iterations = 1000;
constexpr int num_records = 1000;
constexpr int size_unit = 100;
constexpr int check_unit = 100;

double GetWallTime() {
  const auto epoch = std::chrono::time_point<std::chrono::system_clock>();
  const auto current = std::chrono::system_clock::now();
  const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(current - epoch);
  return elapsed.count() / 1000000.0;
}

void* Realloc(bool alignment, void* old_ptr, size_t size) {
  if (alignment) {
    size_t aligned_size = 8;
    while (aligned_size < size) {
      aligned_size *= 2;
    }
    size = aligned_size;
  }
  void* new_ptr = std::realloc(old_ptr, size);
  assert(new_ptr != nullptr);
  return new_ptr;
}

void TestRealloc(bool alignment) {
  std::cout << "-- Realloc alignment=" << alignment << " --" << std::endl;
  void* records[num_records];
  for (int record_index = 0; record_index < num_records; record_index++) {
    records[record_index] = nullptr;
  }
  int size = size_unit;
  double start_time = GetWallTime();
  for (int iteration_count = 1; iteration_count <= num_iterations; iteration_count++) {
    for (int record_index = 0; record_index < num_records; record_index++) {
      records[record_index] = Realloc(alignment, records[record_index], size);
    }
    if (iteration_count % check_unit == 0) {
      const double end_time = GetWallTime();
      std::cout << "iteration=" << iteration_count
                << " size=" << size
                << " time=" << end_time - start_time << std::endl;
      start_time = GetWallTime();
    }
    size += size_unit;
  }
}

void TestString() {
  std::cout << "-- std::string --" << std::endl;
  std::string records[num_records];
  std::string unit(size_unit, 0);
  double start_time = GetWallTime();
  for (int iteration_count = 1; iteration_count <= num_iterations; iteration_count++) {
    for (int record_index = 0; record_index < num_records; record_index++) {
      records[record_index].append(unit);
    }
    if (iteration_count % check_unit == 0) {
      const double end_time = GetWallTime();
      std::cout << "iteration=" << iteration_count
                << " size=" << records[0].size()
                << " time=" << end_time - start_time << std::endl;
      start_time = GetWallTime();
    }
  }
}

int main(int argc, char** argv) {
  std::cout << std::fixed;
  std::cout << std::setprecision(4);
  TestRealloc(false);
  TestRealloc(true);
  TestString();
  return 0;
}
