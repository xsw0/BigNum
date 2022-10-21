#include <iostream>
#include "Natural.h"

using namespace std;

int main() {
  string s;
  Natural sum;
  while (cin >> s) {
    sum += Natural::parse(s).value();
    cout << sum.to_string() << endl;
  }

  std::cout << "Hello, World!" << std::endl;
  return 0;
}
