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
    exit1
  fi
}

try 0 0
try 42 42
try 21 "5+20-4"
try 41 " 12 + 34  -5"
try 47 "5+6*7"
try 8 "5+6/2"
try 16 "+8 +8"
try 10 "-8 +18"
echo OK