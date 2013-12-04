#include <iostream>
#include <vector>
#include <string>
#include "bdd.hpp"
using namespace choro3;

int main() {

  // 1. 共有化 BDD のノードを管理する BDDManager を初期化する
  BDDManager* manager = BDDManager::get_manager();

  // 2. 論理式の作成
  // 文字列をパースして作る場合
  std::string buf; getline(std::cin, buf);
  BDDExpression exp = manager->create_expression(buf);
  // プログラム中で変数から式を組み立てる場合
  // BDDExpression a = manager->add_variable(1);
  // BDDExpression b = manager->add_variable(2);
  // BDDExpression c = manager->add_variable(3);
  // BDDExpression exp = a & b | c;

  // 3. 変数に実際の値を与える
  std::vector<assign_t> as;
  for(int val, i = 1; std::cin >> val; i++) {
    as.push_back(std::make_pair(i, (bool)val));
  }

  // 4. 評価
  std::cout << exp(as) << std::endl;

  return 0; 
}
