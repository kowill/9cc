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

echo OK
