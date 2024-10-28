#!/bin/bash

# Test cases
declare -a expressions=(
  'user'
  'user.name'
  'user.emails[0]'
  'user.address.city'
  'user.age + 5'
  'max(user.scores)'
  'max(user.scores) - min(user.scores)'
  'matrix[1][2]'
  'size(numbers)'
  'size(user.name)'
  'numbers[0] + numbers[4]'
  'products[1].name'
  'products[0].price * 2'
  'size(user.emails[1])'
  '-user.age'
  '(user.scores[0] + user.scores[1] + user.scores[2]) / 3'
  'emptyArray[0]'
  'emptyObject.key'
  'nested.level1.level2.level3'
  'max(numbers[0], numbers[1], numbers[2])'
  'user["name"]'
  'numbers[0] - numbers[3]'
  'products[0].name.size()'
  'max("a", "b")'
  'unknownVar'
  'numbers[1] / (numbers[3] - numbers[3])'
  'numbers["one"]'
  '(user.scores[0] * 0.5) + (user.scores[1] * 0.3) + (user.scores[2] * 0.2)'
  'size(matrix[0])'
  'numbers[-1]'
  'flags.isActive'
  'user.name + " Smith"'
  'matrix[3][0]'
  'min'
  '10 + 20 / 5 * 2'
  '(10 + 20) / (5 * 2)'
  'size()'
  'size(user.name, user.age)'
  'numbers[1 + 1]'
  'user.nickname'
)

echo "-----------------------------------"

# Loop through the test cases
for expr in "${expressions[@]}"; do
  echo "Expression: $expr"
  ./json_eval test.json "$expr"
  echo "-----------------------------------"
done
