#include <iostream>
#include "bdd.hpp"
using namespace choro3;

int main() {

  BDDManager* manager = BDDManager::get_manager();

  while(true) {
    int N; std::cin >> N; std::cin.ignore();
    std::string buf;
    getline(std::cin, buf);
    if(buf == "END") break;
    std::vector<assign_t> as; int val;
    for(int i = 1; i <= N; i++) {
      std::cin >> val;
      as.push_back(std::make_pair(i, val));
    }
    BDDExpression exp = manager->create_expression(buf);
    std::cout << exp(as) << std::endl;
  }

  return 0; 
}
