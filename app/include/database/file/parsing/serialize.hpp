#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>
#include <fstream>

#include "utils/templating.hpp"

template<typename T, typename CONTEXT = void>
struct Serialize {
    static std::streampos toStream(const T &element, std::streampos position, std::fstream &fileStream, CONTEXT context);
};

template<typename T>
struct Serialize<T,void> {
    static std::streampos toStream(const T &element, std::streampos position, std::fstream &fileStream);
};

template<typename T, typename CONTEXT = void>
concept Serializable =
    requires (const T& element, std::streampos position, std::fstream& fileStream) {
        { Serialize<T, CONTEXT>::toStream(element, position, fileStream) } -> std::same_as<std::streampos>;
    } ||
    requires (const T& element, std::streampos position, std::fstream& fileStream, CONTEXT context) {
        { Serialize<T, CONTEXT>::toStream(element, position, fileStream, context) } -> std::same_as<std::streampos>;
    };
