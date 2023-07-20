#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>
#include <fstream>

#include "utils/templating.hpp"

template<typename T>
struct Serialize {
    static std::streampos toStream(const T &element, std::streampos position, std::fstream &fileStream);
};


template<typename T>
concept Serializable = requires (const T& element, std::streampos position, std::fstream& fileStream) {
    { Serialize<T>::toStream(element, position, fileStream) } -> std::same_as<std::streampos>;
};
