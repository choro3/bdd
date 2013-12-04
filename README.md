# bdd.git

BDD / ZDD の勉強用リポジトリ

## 動かし方

```sh
$ make
$ cat sample.in
1 & 2 & 3 | 1 & 2 & 4 | 1 & 3 & 4 | 2 & 3 & 4 # BDDで表したい論理式
1 1 0 1                                       # 変数の値 (左から 1, 2, 3, 4
$ ./bdd < sample.in
1                                             # 上の入力を評価した結果
```

参考: [アルゴリズム特論 講義資料](http://www-alg.ist.hokudai.ac.jp/~minato/alg2006/) (北大・湊先生)
