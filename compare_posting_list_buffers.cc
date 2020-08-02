#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <unordered_map>

#include "tkrzw_cmd_util.h"

constexpr int32_t num_words = 10000000;
constexpr int32_t check_unit = num_words / 10;

std::string MakeRandomWord(std::mt19937* mt, int32_t length) {
  std::uniform_int_distribution<uint64_t> char_dist(0, tkrzw::UINT64MAX);
  const uint64_t num = char_dist(*mt) & 0x0f0f0f0f0f0f0f0f;
  return std::string((char*)&num, length);
}

void TestDBM(tkrzw::DBM* index) {
  std::mt19937 mt(1978);
  const int64_t start_mem_rss = tkrzw::StrToInt(tkrzw::GetSystemInfo()["mem_rss"]);  
  double start_time = tkrzw::GetWallTime();
  for (int64_t i = 1; i <= num_words; i++) {
    const int32_t length = i % 7 + 1;
    const std::string word = MakeRandomWord(&mt, length);
    index->Append(word, std::string_view((char*)&i, sizeof(i)));
    if (i % check_unit == 0) {
      const double end_time = tkrzw::GetWallTime();
      const int64_t current_mem_rss = tkrzw::StrToInt(tkrzw::GetSystemInfo()["mem_rss"]);
      std::cout << i << ": uniq_words=" << index->CountSimple()
                << " time=" << (end_time - start_time)
                << " mem=" << ((current_mem_rss - start_mem_rss) / 1024.0 / 1024.0)
                << std::endl;
      start_time = tkrzw::GetWallTime();
    }
  }
}

template <typename MAP>
void TestMap(MAP* index) {
  std::mt19937 mt(1978);
  const int64_t start_mem_rss = tkrzw::StrToInt(tkrzw::GetSystemInfo()["mem_rss"]);  
  double start_time = tkrzw::GetWallTime();
  for (int64_t i = 1; i <= num_words; i++) {
    const int32_t length = i % 7 + 1;
    const std::string word = MakeRandomWord(&mt, length);
    (*index)[word].append((char*)&i, sizeof(i));
    if (i % check_unit == 0) {
      const double end_time = tkrzw::GetWallTime();
      const int64_t current_mem_rss = tkrzw::StrToInt(tkrzw::GetSystemInfo()["mem_rss"]);
      std::cout << i << ": uniq_words=" << index->size()
                << " time=" << (end_time - start_time)
                << " mem=" << ((current_mem_rss - start_mem_rss) / 1024.0 / 1024.0)
                << std::endl;
      start_time = tkrzw::GetWallTime();
    }
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Mode is not specified." << std::endl;
    return 1;
  }
  const std::string mode(argv[1]);
  std::cout << std::fixed;
  std::cout << std::setprecision(4);
  if (mode == "tiny") {
    std::cout << "-- TinyDBM --" << std::endl;
    tkrzw::TinyDBM index(num_words);
    TestDBM(&index);
  } else if (mode == "baby") {
    std::cout << "-- BabyDBM --" << std::endl;
    tkrzw::BabyDBM index;
    TestDBM(&index);
  } else if (mode == "unordered_map") {
    std::cout << "-- std::unordered_map --" << std::endl;
    std::unordered_map<std::string, std::string> index;
    index.reserve(num_words);
    TestMap(&index);
  } else if (mode == "map") {
    std::cout << "-- std::map --" << std::endl;
    std::map<std::string, std::string> index;
    TestMap(&index);
  } else {
    std::cerr << "Unknown mode." << std::endl;
  }
  return 0;
}
