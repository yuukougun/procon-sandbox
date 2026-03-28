#include "sample_math.hpp"

#include <numeric>

namespace sample {

int SampleMath::sum(const std::vector<int>& values) {
    return std::accumulate(values.begin(), values.end(), 0);
}

std::string SampleMath::greet(const std::string& name) {
    return "Hi, my name is yuki" + name;
}

}  // namespace sample
