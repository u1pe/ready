#include <iostream>

int main() {
  int i = 1;
  for (int j = 0; j < 100; j++) {
    i += j;
    if (i > 100) {
      break;
    }
  }
  std::cout << i << std::endl;
}