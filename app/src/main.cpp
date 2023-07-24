#include <boost/uuid/uuid.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "database/RecipeDatabase.hpp"
#include "database/file/FileBackedBPlusTree.hpp"
#include "defaultConfig.hpp"

#ifndef TESTING

int main() {
  std::cout << "Ingredients tree with order: " << INGREDIENT_ORDER << std::endl;
  std::cout << "Recipes tree with order: " << RECIPES_ORDER << std::endl;
  FileBackedBPlusTree<FixedLengthIngredient, ADDRESS_TYPE> ingredientTree(
      "./ingredients.graph_index", "./ingredients.graph_db", INGREDIENT_ORDER,
      BLOCK_SIZE, true);
  FileBackedBPlusTree<boost::uuids::uuid, ADDRESS_TYPE> recipeTree(
      "./recipes.graph_index", "./recipes.graph_db", RECIPES_ORDER, BLOCK_SIZE,
      true);

  RecipeDatabase<MAX_STRING_LENGTH, ADDRESS_TYPE> recipeDatabase(ingredientTree,
                                                                 recipeTree);

  FixedLengthIngredient peppers("peppers");
  FixedLengthIngredient rice("rice");
  FixedLengthIngredient ricePudding("rice pudding");
  FixedLengthIngredient custard("custard");

  auto recipe1 = Recipe("fry off peppers and add some rice");
  recipeDatabase.addRecipe({peppers, rice}, recipe1);
  auto recipe2 = Recipe("fry off peppers and add some rice but differently");
  recipeDatabase.addRecipe({peppers, rice}, recipe2);

  auto recipes = recipeDatabase.getRecipes(peppers);
  std::cout << recipes.value()[0].getContents() << std::endl;
  std::cout << recipes.value()[1].getContents() << std::endl;
}

#endif