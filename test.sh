#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" != "$expected" ]; then
    echo "$input expected $expected, but got $actual"
    exit 1
  fi
}

try 0 0
try 192 192
try 255 255

try 10 2+8
try 2 8-6

try 6 1+2+3

try 30 " 6 + 3+21 "

try 12 3*4
try 15 "3 * 5"

try 7 77/11
try 3 " 12/4"

echo OK
