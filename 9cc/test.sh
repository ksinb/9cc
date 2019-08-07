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

try 0 "main(){return 0;}"
try 42 "main(){return 42;}"
try 5 "main(){return 2 + 3;}"
try 21 "main(){return 5+20-4;}"
try 41 "main(){ return 12 + 34  -5; }"
try 47 "main(){ return 5+6*7; }"
try 2 "main(){ return ( 2 ) ; }"
try 16 "main(){ return (2 + 6) * 2; }"
try 4 "main(){ return (2 + 6) / 2; }"
try 16 "main(){ return +8 +8; }"
try 10 "main(){ return -8 +18; }"
try 1 "main() { return 3 == 3; }"
try 1 "main() { return 3 != 5; }"
try 1 "main() { return 3 < 5; }"
try 1 "main() { return 3 <= 3; }"
try 0 "main() { return 3 > 5; }"
try 0 "main() { return (1 + 2) > (3 + 5); }"
try 3 "main() { a = 3; return a; }"
try 5 "main() { a = 3; return a + 2; }"
try 6 "main() { a=3; return 2*a; }"
try 10 "main() { a=1; a=2; a=10; return a; }"
try 15 "main() { a = 4; b = 5 * 6 - 8; return a + b / 2; }"
try 30 "main() { ab = 10; cd = 20; return ab+cd; }"
try 3 "main() { returned = 3; return returned; }"
try 14 "main() { a = 3; b = 5 * 6 - 8; return a + b / 2; }"
try 5 "main() { a = 10; if (1 == 2) a = 20; return a-5; }"
try 25 "main() { a = 0; if (1 == 2) a = 20; else a = 30; return a-5; }"
try 5 "main() { a = 0; while (a < 5) a = a + 1; return a; }"
try 15 "main() { a = 10; i = 0; for (i = 0; i < 5; i = i+1) a = a + 1; return a; }"
try 5 "main() { i = 0; for (; i < 5; ) i = i + 1; return i; }"
try 6 "main() { i = 3; for (; ; i = i + 1) if (i > 5) return i; }"
try 3 "main() { { a = 1; b = 2; c = 3; } return 3; }"
try 3 "main() { if (1 < 2) { a = 3; return 3; } else { b = 4; return 4; } }"
try 55 'main() {return foo1(55);}'
try 3  'main() {return foo2(1, 2);}'
try 6  'main() {return foo3(1, 2, 3);}'
try 10 'main() {return foo4(1, 2, 3, 4);}'
try 15 'main() {return foo5(1, 2, 3, 4, 5);}'
try 21 'main() {return foo6(1, 2, 3, 4, 5, 6);}'
try 12 'main() {a = foo1(1); b = foo2(2, 3); c = foo3(1, 2, 3); return a + b + c;}'
try 42 "foo(){ return 42; } main(){ a = foo(); return a;}"
try 22 "plus2(a){ return a + 2; } main(){ a = plus2(20); return a; }"
try 10 "sum(a, b){ return a + b; } main(){ c = sum(3, 7); return c; }"
try 21 "sum(a, b, c, d, e, f){ return a+b+c+d+e+f; } main(){ c = sum(1,2,3,4,5,6); return c; }"
try 55 "main() { x = 55; y = &x; return *y; }"
echo "all passed"