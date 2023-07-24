#pragma once

#include <string>

#include "database/file/parsing/common.hpp"

class Recipe {
  std::string contents;

public:
  explicit Recipe(std::string contents);
  explicit Recipe(const char *contents);
  [[nodiscard]] const std::string &getContents() const;
};

template <> struct Deserialize<Recipe> {
  static Recipe fromStream(std::streampos position, std::fstream &fileStream);
};

template <> struct Serialize<Recipe> {
  static std::streampos toStream(const Recipe &dataChunk,
                                 std::streampos position,
                                 std::fstream &fileStream);
};