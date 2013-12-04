/* 
/ bdd.hpp - 
/
/ description.
/
/ 2013-12-05 05:21:05 choro3
*/

#ifndef __CHORO3_BDD_H
#define __CHORO3_BDD_H

#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <iostream>

namespace choro3 {

  // 変数型
  typedef int variable_t;

  // 変数と実際の値の対応
  typedef std::pair<variable_t, int> assign_t;

  // 付値から v に対応するものを得る
  bool get_value(variable_t v, const std::vector<assign_t>& as);

  // 論理演算 ( BDDManager::apply_binary_op 用)
  enum BDDOperation { AND, OR, NOT, XOR };

  // 定数ノード用のダミーのインデックス
  static const int BDD_TRUE_INDEX  = -1;
  static const int BDD_FALSE_INDEX = -2;
  static const int BDD_NULL_INDEX  = -3;

  // BDDNode:
  struct BDDNode {

    friend class BDDExpression;
    friend class BDDManager;

    // ノードに対応する変数
    variable_t var;

    // 対応する変数が真の場合 hi, 偽の場合 lo
    int hi, lo;

    BDDNode() {}

    BDDNode(int v, int h, int l): var(v), hi(h), lo(l) {}

    bool operator == (const BDDNode& rhs) const {
      return var == rhs.var && lo == rhs.lo && hi == rhs.hi;
    }
  };

  class BDDManager;

  // BDDExpression:
  //   BDD での論理式を表す。
  //   実態は共有化 BDD クラス BDDManager の持つノードへのポインタ。
  class BDDExpression {

    friend class BDDManager;

    // この式(が持つノード)を管理する BDDManager
    BDDManager& manager;

    // 式のルートノード
    int index;

    BDDExpression();
    BDDExpression(BDDManager& manager, int root_index);
    BDDExpression(BDDManager& manager, variable_t v, const BDDExpression& hi, const BDDExpression& lo);

  public:

    // 各種論理演算
    const BDDExpression& operator &= (const BDDExpression& rhs);
    const BDDExpression& operator |= (const BDDExpression& rhs);
    const BDDExpression& operator ^= (const BDDExpression& rhs);
    BDDExpression operator ! () const;
    BDDExpression operator & (const BDDExpression& rhs) const;
    BDDExpression operator | (const BDDExpression& rhs) const;
    bool operator == (const BDDExpression& rhs) const;

    // assign を付値として式を評価
    bool operator () (const std::vector<assign_t>& assign);

    // TODO: implement this
    std::string to_string();
  };

  // BDDManager: 
  //   BDD 形式の論理式 BDDExpression のファクトリクラス。
  //   共有化 BDD のノードも保持する
  class BDDManager {

    friend class BDDExpression;

    static BDDManager* instance;

    // ノード配列
    std::vector<BDDNode> nodes;

    // 変数の順序
    std::vector<variable_t> order;

    // 定数ノード(番兵)
    BDDExpression BDD_TRUE, BDD_FALSE;

    BDDManager();

    // bdd が定数式か
    bool const_expression(const BDDExpression& bdd);

    // var1 var2 のうち順序が早い方を返す
    variable_t choose_high_priority(const variable_t var1, const variable_t var2);

    // exp が指す BDD の、ｖよりも順序が後ろにある変数からなる部分木(?)
    // side == true なら v が真の場合, false なら v が偽の場合
    BDDExpression subtree(const BDDExpression& exp, const variable_t& v, bool side);

    // 文字列で表された論理式のパース
    char _next_token(const std::string& exp, int& p);
    BDDExpression _parse_exp(const std::string& exp, int& p);
    BDDExpression _parse_or(const std::string& exp, int& p);
    BDDExpression _parse_and(const std::string& exp, int& p);
    BDDExpression _parse_not(const std::string& exp, int& p);
    BDDExpression _parse_unit(const std::string& exp, int& p);
    BDDExpression _parse_var(const std::string& exp, int& p);

  public:

    // 定数ノードとの二項演算
    BDDExpression apply_with_constexp(BDDOperation op, const BDDExpression& exp, const BDDExpression& constexp);

    // 論理二項演算の適用
    BDDExpression apply_binary_op(BDDOperation op, const BDDExpression& lhs, const BDDExpression& rhs);

    // 与えられた引数と等価なノードがあればノード配列上のインデックスを返す
    int get_node(const variable_t& v, const BDDExpression& hi, const BDDExpression& lo);

    // 変数 v のみからなる論理式の BDD を生成
    BDDExpression add_variable(variable_t v);

    // BDD のノードをすべて出力
    void dumps() const;

    // 文字列をパースして BDD に
    BDDExpression create_expression(const std::string& exp);

    // 参照されていないノードの削除
    // TODO: implement this
    static void sweep();

    // singleton のインスタンス取得
    static BDDManager* get_manager();
  };
};

#endif
