#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "database/file/FileBackedBPlusTree.hpp"

#include "model/Ingredient.hpp"
#include "model/Recipe.hpp"

#include "utils/bitwise.hpp"

template <size_t MAX_LENGTH, typename ADDRESS> class RecipeDatabase {
private:
  FileBackedBPlusTree<Ingredient<MAX_LENGTH>, ADDRESS> ingredientTree;
  FileBackedBPlusTree<boost::uuids::uuid, ADDRESS> recipeTree;
  boost::uuids::random_generator uuidGenerator{};

public:
  RecipeDatabase(
      FileBackedBPlusTree<Ingredient<MAX_LENGTH>, ADDRESS> &ingredientTree,
      FileBackedBPlusTree<boost::uuids::uuid, ADDRESS> &recipeTree)
      : ingredientTree(std::move(ingredientTree)),
        recipeTree(std::move(recipeTree)) {}

  void addRecipe(const std::vector<Ingredient<MAX_LENGTH>> &ingredients,
                 const Recipe &recipe) {
    auto recipeId = uuidGenerator();

    std::vector<uint8_t> recipeBuffer(recipe.getContents().length());
    std::memcpy(recipeBuffer.data(), recipe.getContents().data(),
                recipe.getContents().length());

    recipeTree.insert(recipeId, DataChunk<ADDRESS>(recipeBuffer));

    for (const auto &ingredient : ingredients) {
      auto ingredientData = ingredientTree.find(ingredient);
      if (ingredientData) {
      } else {
        DataChunk<ADDRESS> recipeData({});
        ingredientTree.insert(ingredient, recipeData);
      }
    }
  }

  std::vector<Recipe> getRecipes(const Ingredient<MAX_LENGTH> &ingredient) {
    return {};
  }
};

template <> struct Serialize<boost::uuids::uuid> {
  FIXED_LENGTH_SERIALIZER(boost::uuids::uuid, 16) {
    std::array<uint8_t, 16> uuidBuffer{0};
    std::memcpy(uuidBuffer.data(), it.data, 16);
    return uuidBuffer;
  }
};

template <> struct Deserialize<boost::uuids::uuid> {
  FIXED_LENGTH_DESERIALIZER(boost::uuids::uuid, 16) {
    boost::uuids::uuid uuid{};
    std::memcpy(&uuid, it.data(), 16);

    return uuid;
  }
};
