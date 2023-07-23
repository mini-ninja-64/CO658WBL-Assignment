#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "database/file/FileBackedBPlusTree.hpp"

#include "model/Ingredient.hpp"
#include "model/Recipe.hpp"

#include "utils/bitwise.hpp"

template <size_t RECIPE_NAME_LENGTH> class RecipeDatabase {
private:
  FileBackedBPlusTree<Ingredient<RECIPE_NAME_LENGTH>, uint32_t> ingredientTree;
  FileBackedBPlusTree<boost::uuids::uuid, uint32_t> recipeTree;
  boost::uuids::random_generator uuidGenerator{};

public:
  RecipeDatabase(FileBackedBPlusTree<Ingredient<RECIPE_NAME_LENGTH>, uint32_t>
                     &ingredientTree,
                 FileBackedBPlusTree<boost::uuids::uuid, uint32_t> &recipeTree)
      : ingredientTree(std::move(ingredientTree)),
        recipeTree(std::move(recipeTree)) {}

  void addRecipe(const std::vector<Ingredient<RECIPE_NAME_LENGTH>> &ingredients,
                 const Recipe &recipe) {
    auto recipeId = uuidGenerator();

    std::vector<uint8_t> recipeBuffer(recipe.getContents().length());
    std::memcpy(recipeBuffer.data(), recipe.getContents().data(),
                recipe.getContents().length());

    recipeTree.insert(recipeId, DataChunk(recipeBuffer));

    for (const auto &ingredient : ingredients) {
      auto ingredientData = ingredientTree.find(ingredient);
      if (ingredientData) {
      } else {
        DataChunk recipeData({});
        ingredientTree.insert(ingredient, recipeData);
      }
    }
  }

  std::vector<Recipe> getRecipes(uint32_t ingredientId) { return {}; }
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
