#!/bin/bash
try() {
  expected=$1
  input=$2

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else 
    echo "$input => $expected expected, but got $actual"
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
echo OK