#include <iostream>
#include <vector>
#include <string>
#include "bdd.hpp"
using namespace choro3;

int main() {

  // 1. ��ͭ�� BDD �ΥΡ��ɤ�������� BDDManager ����������
  BDDManager* manager = BDDManager::get_manager();

  // 2. �������κ���
  // ʸ�����ѡ������ƺ����
  std::string buf; getline(std::cin, buf);
  BDDExpression exp = manager->create_expression(buf);
  // �ץ��������ѿ����鼰���Ȥ�Ω�Ƥ���
  // BDDExpression a = manager->add_variable(1);
  // BDDExpression b = manager->add_variable(2);
  // BDDExpression c = manager->add_variable(3);
  // BDDExpression exp = a & b | c;

  // 3. �ѿ��˼ºݤ��ͤ�Ϳ����
  std::vector<assign_t> as;
  for(int val, i = 1; std::cin >> val; i++) {
    as.push_back(std::make_pair(i, (bool)val));
  }

  // 4. ɾ��
  std::cout << exp(as) << std::endl;

  return 0; 
}
