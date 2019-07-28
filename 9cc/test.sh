#!/bin/bash
try() {
  expected=$1
  input=$2

  ./9cc "$input" > tmp.s
    gcc -static -o tmp tmp.s test/*.o
  ./tmp
  actual="$?"
  
  if [ "$actual" = "$expected" ]; then
    echo "OK: $input => $actual"
  else 
    echo "NG: $input => $expected expected, but got $actual"
    exit 1
  fi
}

try 0 "0;"
try 42 "42;"
try 5 "2 + 3;"
try 21 "5+20-4;"
try 41 " 12 + 34  -5;"
try 47 "5+6*7;"
try 2 "( 2 ) ;"
try 16 "(2 + 6) * 2;"
try 4 "(2 + 6) / 2;"
try 16 "+8 +8;"
try 10 "-8 +18;"
try 1 "3 == 3;"
try 1 "3 != 5;"
try 1 "3 < 5;"
try 1 "3 <= 3;"
try 0 "3 > 5;"
try 0 "(1 + 2) > (3 + 5);"
try 3 "a = 3; a;"
try 5 "a = 3; a + 2;"
try 6 "a=3; 2*a;"
try 15 "a = 4; b = 5 * 6 - 8; a + b / 2;"
try 30 "ab = 10; cd = 20; ab+cd;"
try 3 "returned = 3; return returned;"
try 14 "a = 3; b = 5 * 6 - 8; return a + b / 2;"
try 5 "a = 10; if (1 == 2) a = 20; return a-5;"
try 25 "a = 0; if (1 == 2) a = 20; else a = 30; return a-5;"
try 5 "a = 0; while (a < 5) a = a + 1; return a;"
try 15 "a = 10; i = 0; for (i = 0; i < 5; i = i+1) a = a + 1; return a;"
try 5 "i = 0; for (; i < 5; ) i = i + 1; return i;"
try 6 "i = 3; for (; ; i = i + 1) if (i > 5) return i;"
try 3 "{ a = 1; b = 2; c = 3; } return 3;"
try 3 "if (1 < 2) { a = 3; return 3; } else { b = 4; return 4; }"
try 55 '{return foo1(55);}'
try 3  '{return foo2(1, 2);}'
try 6  '{return foo3(1, 2, 3);}'
try 10 '{return foo4(1, 2, 3, 4);}'
try 15 '{return foo5(1, 2, 3, 4, 5);}'
try 21 '{return foo6(1, 2, 3, 4, 5, 6);}'
try 12 '{a = foo1(1); b = foo2(2, 3); c = foo3(1, 2, 3); return a + b + c;}'

echo "all passed"