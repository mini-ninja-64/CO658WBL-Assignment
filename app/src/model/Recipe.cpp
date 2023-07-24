#include "model/Recipe.hpp"

#include <utility>

#include "utils/bitwise.hpp"

Recipe::Recipe(std::string contents) : contents(std::move(contents)) {}
const std::string &Recipe::getContents() const { return contents; }
Recipe::Recipe(const char *contents) : contents(contents) {}

std::streampos Serialize<Recipe>::toStream(const Recipe &recipe,
                                           std::streampos position,
                                           std::fstream &fileStream) {
  std::array<uint8_t, 4> stringLengthBuffer = {
      UINT32_TO_UINT8(recipe.getContents().length())};
  fileStream.seekp(position);
  fileStream.write(reinterpret_cast<const char *>(stringLengthBuffer.data()),
                   4);
  fileStream.write(recipe.getContents().data(), recipe.getContents().length());

  auto chunkLength = 4 + recipe.getContents().length();

  return position + std::streamoff(chunkLength);
}

Recipe Deserialize<Recipe>::fromStream(std::streampos position,
                                       std::fstream &fileStream) {
  fileStream.seekg(position);
  std::array<uint8_t, 4> stringLengthBuffer = {0};
  fileStream.read(reinterpret_cast<char *>(stringLengthBuffer.data()), 4);

  auto recipeLength = UINT8_TO_UINT32(stringLengthBuffer, 0);
  std::string recipeContents;
  recipeContents.resize(recipeLength + 1);
  fileStream.read(reinterpret_cast<char *>(recipeContents.data()),
                  recipeLength);
  recipeContents[recipeLength] = 0;
  return Recipe(recipeContents);
}