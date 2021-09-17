#include "test_helper.h"

namespace toystl {
namespace testhelper {
bool operator==(const nontrivial& lhs, const nontrivial& rhs) {
  return *lhs.i_ == *rhs.i_ && lhs.j_ == rhs.j_;
}

bool operator!=(const nontrivial& lhs, const nontrivial& rhs) {
  return !(lhs == rhs);
}
}  // namespace TestHelper
}  // namespace toystl