#ifndef TOYSTL_PERFORMANCE_PERFORM_VECTOR_H_
#define TOYSTL_PERFORMANCE_PERFORM_VECTOR_H_

#include <unistd.h>
#include <functional>
#include <string>
#include <vector>

#include "profiler.h"
#include "vector.h"

namespace toystl {
namespace profiler {
void vector_perform() {
  {
    std::cout << "[----------------- Run Vector performance test "
                 "-----------------]\n";
    std::cout << "|---------------------|-------------|-------------|----------"
                 "---|\n";
    std::cout << "|      push_back      |    500000   |   5000000   |   "
                 "50000000  |\n";
    std::cout << "[---------------------------- toystl "
                 "----------------------------]\n";
    int count = 500000;
    // toystl
    toystl::vector<int> mvec;
    // 1
    ProfilerInstance::start();
    for (int i = 0; i != count; ++i) mvec.push_back(1);
    ProfilerInstance::end();
    ProfilerInstance::dumpDuringTime();
    // 2
    ProfilerInstance::start();
    for (int i = 0; i != count * 10; ++i) mvec.push_back(1);
    ProfilerInstance::end();
    ProfilerInstance::dumpDuringTime();
    // 3
    ProfilerInstance::start();
    for (int i = 0; i != count * 100; ++i) mvec.push_back(1);
    ProfilerInstance::end();
    ProfilerInstance::dumpDuringTime();
    std::cout << "\n";
  }
  std::cout
      << "[----------------------------- std -----------------------------]\n";
  int count = 500000;
  // std
  std::vector<int> svec;
  // 1
  ProfilerInstance::start();
  for (int i = 0; i != count; ++i) svec.push_back(1);
  ProfilerInstance::end();
  ProfilerInstance::dumpDuringTime();
  // 2
  ProfilerInstance::start();
  for (int i = 0; i != count * 10; ++i) svec.push_back(1);
  ProfilerInstance::end();
  ProfilerInstance::dumpDuringTime();
  // 3
  ProfilerInstance::start();
  for (int i = 0; i != count * 100; ++i) svec.push_back(1);
  ProfilerInstance::end();
  ProfilerInstance::dumpDuringTime();
  std::cout << "\n";
  std::cout
      << "[---------------------------------------------------------------]\n";
}
}  // namespace profiler
}  // namespace toystl
#endif  // TOYSTL_PERFORMANCE_PERFORM_VECTOR_H_