#pragma once

#include "model/Ingredient.hpp"
#include "database/file/parsing/baseParsers.hpp"

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
#define INGREDIENT_ORDER                                                       \
  (int)((BLOCK_SIZE - 5 - (3 * ADDRESS_SIZE)) /                                \
        (INGREDIENT_KEY_SIZE + ADDRESS_SIZE))

#define RECIPES_KEY_SIZE Serialize<boost::uuids::uuid>::length
#define RECIPES_ORDER                                                          \
  (int)((BLOCK_SIZE - 5 - (3 * ADDRESS_SIZE)) /                                \
        (RECIPES_KEY_SIZE + ADDRESS_SIZE))

typedef Ingredient<MAX_STRING_LENGTH> FixedLengthIngredient;
