#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>
#include <fstream>

#include "utils/templating.hpp"

template<typename T, typename CONTEXT = void>
struct Deserialize {
    static T fromStream(std::streampos position, std::fstream& fileStream, CONTEXT context);
};

template<typename T>
struct Deserialize<T,void> {
    static std::streampos toStream(const T &element, std::streampos position, std::fstream &fileStream);
};

template<typename T, typename CONTEXT = void>
concept Deserializable =
    requires (std::streampos position, std::fstream& fileStream) {
        { Deserialize<T>{}.fromStream(position, fileStream)} -> std::same_as<T>;
    } ||
    requires (std::streampos position, std::fstream& fileStream, CONTEXT context) {
        { Deserialize<T>::fromStream(position, fileStream, context) } -> std::same_as<T>;
    };
