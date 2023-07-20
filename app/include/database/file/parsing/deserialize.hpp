#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>
#include <fstream>

#include "utils/templating.hpp"

template<typename T>
struct Deserialize {
    static T fromStream(std::streampos position, std::fstream& fileStream);
};

template<typename T>
concept Deserializable = requires (std::streampos position, std::fstream& fileStream) {
    { Deserialize<T>{}.fromStream(position, fileStream) } -> std::same_as<T>;
};
