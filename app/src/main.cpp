#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "database/RecipeDatabase.hpp"
#include "database/file/FileBackedBPlusTree.hpp"

#ifndef TESTING

#define BLOCK_SIZE 4096
#define ADDRESS_TYPE uint32_t

/*
 * Calculating order
 *
 * A = Address Size
 * K = Key Size
 * O = Graph Order
 *
 * Take the block size (B) and use it to maximise the order like so:
 *
 * B = 5 + 3A + O(K + A)
 * O = (B - 5 - 3A) / (K + A)
 *
 * Typically modern OS use a block size of 4096
 *
 * So if our DB was using 32-bit addressing with a 32-bit key size
 * the optimal order would be = (4096-5-(3*4)) / (4 + 4) = ~509
 *
 */

#define ADDRESS_SIZE Serialize<ADDRESS_TYPE>::length

#define MAX_STRING_LENGTH 20
#define INGREDIENT_KEY_SIZE Serialize<Ingredient<MAX_STRING_LENGTH>>::length
#define INGREDIENT_ORDER (int)((BLOCK_SIZE - 5 - (3*ADDRESS_SIZE))/(INGREDIENT_KEY_SIZE + ADDRESS_SIZE))

#define RECIPES_KEY_SIZE Serialize<boost::uuids::uuid>::length
#define RECIPES_ORDER (int)((BLOCK_SIZE - 5 - (3*ADDRESS_SIZE))/(RECIPES_KEY_SIZE + ADDRESS_SIZE))

typedef Ingredient<MAX_STRING_LENGTH> FixedLengthIngredient;
int main() {
  std::cout << "Ingredients tree with order: " << INGREDIENT_ORDER << std::endl;
  std::cout << "Recipes tree with order: " << RECIPES_ORDER << std::endl;
  FileBackedBPlusTree<FixedLengthIngredient, ADDRESS_TYPE> ingredientTree(
      "./ingredients.graph_index", "./ingredients.graph_db", INGREDIENT_ORDER, BLOCK_SIZE,
      true);
  FileBackedBPlusTree<boost::uuids::uuid, ADDRESS_TYPE> recipeTree(
      "./recipes.graph_index", "./recipes.graph_db", RECIPES_ORDER, BLOCK_SIZE, true);

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