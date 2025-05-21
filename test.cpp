//
// Created by Alexandr Kaminskiy on 21.05.2025.
//

#include "../../../../Library/Developer/CommandLineTools/SDKs/MacOSX14.4.sdk/usr/include/c++/v1/set"
#include <iostream>
int main() {
  std::set<int> numbers;
  numbers.insert(1);
  numbers.insert(2);
  numbers.insert(3);

  if (numbers.count(10)) {
    std::cout << numbers.count(1) << std::endl;
  }
}
