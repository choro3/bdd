#include "bdd.hpp"

namespace choro3 {

  bool get_value(variable_t v, const std::vector<assign_t>& as) {
    // TODO: more efficient
    for(int i = 0; i < (int)as.size(); i++) {
      if(as[i].first == v) return (bool)as[i].second;
    }
    return false;
  }

  //
  // BDDExpression
  // 

  BDDExpression::BDDExpression(BDDManager& manager, int root_index): manager(manager), index(root_index) {}

  BDDExpression::BDDExpression(BDDManager& manager, variable_t v, const BDDExpression& hi, const BDDExpression& lo): manager(manager), index(manager.get_node(v, hi, lo)) {
    if(index == BDD_NULL_INDEX) {
      manager.nodes.push_back(BDDNode(v, hi.index, lo.index));
      index = (int)manager.nodes.size() - 1;
    }
  }

  const BDDExpression& BDDExpression::operator &= (const BDDExpression& rhs) {
    index = manager.apply_binary_op(AND, *this, rhs).index;
    return *this; 
  }

  const BDDExpression& BDDExpression::operator |= (const BDDExpression& rhs) {
    index = manager.apply_binary_op(OR, *this, rhs).index;
    return *this; 
  }

  const BDDExpression& BDDExpression::operator ^= (const BDDExpression& rhs) {
    index = manager.apply_binary_op(XOR, *this, rhs).index;
    return *this; 
  }

  BDDExpression BDDExpression::operator ! () const {
    // TODO: implement this
    return *this;
  }

  BDDExpression BDDExpression::operator & (const BDDExpression& rhs) const {
    BDDExpression res = *this;
    res &= rhs;
    return res; 
  }

  BDDExpression BDDExpression::operator | (const BDDExpression& rhs) const {
    BDDExpression res = *this;
    res |= rhs;
    return res; 
  }

  bool BDDExpression::operator == (const BDDExpression& rhs) const {
    return index == rhs.index;
  }

  bool BDDExpression::operator () (const std::vector<assign_t>& assign) {
    int p = index;    
    while(p >= 0) {
      BDDNode v = manager.nodes[p];
      p = get_value(v.var, assign) ? v.hi : v.lo;
    }
    return p == BDD_TRUE_INDEX;
  }

  std::string BDDExpression::to_string() { return ""; }

  //
  // BDDManager
  //

  BDDManager::BDDManager(): BDD_TRUE(*this, BDD_TRUE_INDEX), BDD_FALSE(*this, BDD_FALSE_INDEX) {}

  bool BDDManager::const_expression(const BDDExpression& bdd) {
    return bdd.index < 0; 
  }

  variable_t BDDManager::choose_high_priority(const variable_t var1, const variable_t var2) {
    // TODO: more efficient
    for(int i = 0; i < (int)order.size(); i++) {
      if(order[i] == var1) return var1;
      if(order[i] == var2) return var2;
    }
    return var1;
  }

  BDDExpression BDDManager::subtree(const BDDExpression& exp, const variable_t& v, bool side) {
    return nodes[exp.index].var == v ? BDDExpression(*this, side ? nodes[exp.index].hi : nodes[exp.index].lo) : exp;
  }

  BDDExpression BDDManager::apply_with_constexp(BDDOperation op, const BDDExpression& exp, const BDDExpression& constexp) {
    switch(op) {
    case AND:
      return constexp == BDD_TRUE ? exp : BDD_FALSE;
    case OR:
      return constexp == BDD_TRUE ? BDD_TRUE : exp;
    case XOR:
      return constexp == BDD_TRUE ? !exp : exp;
    default: 
      throw;
    }
    // cant reach
    return BDDExpression(*this, -1);
  }

  BDDExpression BDDManager::apply_binary_op(BDDOperation op, const BDDExpression& lhs, const BDDExpression& rhs) {

    if(const_expression(rhs)) return apply_with_constexp(op, lhs, rhs);
    if(const_expression(lhs)) return apply_with_constexp(op, rhs, lhs);

    variable_t current_var = choose_high_priority(nodes[lhs.index].var, nodes[rhs.index].var);
    BDDExpression hi = apply_binary_op(op,
                                       subtree(lhs, current_var, true),
                                       subtree(rhs, current_var, true));
    BDDExpression lo = apply_binary_op(op,
                                       subtree(lhs, current_var, false),
                                       subtree(rhs, current_var, false));

    return hi == lo ? hi : BDDExpression(*this, current_var, hi, lo);
  }

  BDDExpression BDDManager::add_variable(variable_t v) {
    // TODO: remove duplication
    nodes.push_back(BDDNode(v, BDD_TRUE_INDEX, BDD_FALSE_INDEX));
    return BDDExpression(*this, nodes.size() - 1);
  }

  BDDExpression BDDManager::create_expression(const std::string& exp) {
    int p = 0;
    return BDDManager::_parse_exp(exp, p);
  }

  int BDDManager::get_node(const variable_t& v, const BDDExpression& hi, const BDDExpression& lo) {
    // TODO: hash
    for(int i = nodes.size() - 1; i >= 0; i--) {
      if(nodes[i].var == v && nodes[i].hi == hi.index && nodes[i].lo == lo.index) {
        return i;
      }
    }
    return BDD_NULL_INDEX;
  } 

  void BDDManager::dumps() const {
    for(int i = 0; i < (int)nodes.size(); i++) {
      std::cerr << i << ": "
                << "var=" << nodes[i].var << ", "
                << "hi=" << nodes[i].hi << ", "
                << "lo=" << nodes[i].lo << std::endl;
    }
  }

  void BDDManager::sweep() {
    // TODO: Implement this.
  }

  BDDManager* BDDManager::instance = NULL;

  BDDManager* BDDManager::get_manager() {
    if(BDDManager::instance == NULL) BDDManager::instance = new BDDManager();
    return BDDManager::instance;
  }

  // パーサー /////

  char BDDManager::_next_token(const std::string& exp, int& p) {
    while(exp[p] == ' ') p++;
    return exp[p];
  }

  BDDExpression BDDManager::_parse_var(const std::string& exp, int& p) {
    int v = 0;
    while(p < (int)exp.length() && isdigit(_next_token(exp, p))) v = v * 10 + exp[p++] - '0';
    return BDDExpression(*this, (variable_t)v, BDD_TRUE, BDD_FALSE);
  }

  BDDExpression BDDManager::_parse_unit(const std::string& exp, int& p) {
    if(_next_token(exp, p) == '(') {
      BDDExpression res = _parse_exp(exp, ++p); p++; // ')'
      return res;
    } else {
      return _parse_var(exp, p);
    }
  }

  BDDExpression BDDManager::_parse_not(const std::string& exp, int& p) {
    if(_next_token(exp, p) == '!') {
      BDDExpression res = _parse_not(exp, ++p);
      return !res;
    } else {
      return _parse_unit(exp, p);
    }
  }

  BDDExpression BDDManager::_parse_and(const std::string& exp, int& p) {
    BDDExpression res = _parse_not(exp, p);
    while(p < (int)(exp.length()) && _next_token(exp, p) == '&') {
      res &= _parse_not(exp, ++p);
    }
    return res;
  }

  BDDExpression BDDManager::_parse_or(const std::string& exp, int& p) {
    BDDExpression res = _parse_and(exp, p);
    while(p < (int)(exp.length()) && _next_token(exp, p) == '|') {
      res |= _parse_and(exp, ++p);
    }
    return res;
  }

  BDDExpression BDDManager::_parse_exp(const std::string& exp, int& p) {
    return _parse_or(exp, p);
  }

  /////////////////
};

