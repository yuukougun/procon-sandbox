#pragma once

#include <string>
#include <vector>

namespace sample {

/**
 * @brief Utility class for small arithmetic examples.
 *
 * This class is intended for documentation preview on GitHub Pages.
 */
class SampleMath {
public:
    /**
     * @brief Return the sum of all values.
     * @param values Input integer list.
     * @return Sum of values. Returns 0 when the list is empty.
     */
    static int sum(const std::vector<int>& values);

    /**
     * @brief Build a short greeting message.
     * @param name Target name.
     * @return Greeting text in the format "Hello, <name>".
     */
    static std::string greet(const std::string& name);
};

}  // namespace sample
