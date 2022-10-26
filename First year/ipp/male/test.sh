#!/bin/bash
prog="$1"
dir="$2"
for f in "$dir"/*.in
do
  valgrind --log-file=t>/dev/null --error-exitcode=32 "./$prog" < "$f" > "tmp.out" 2>"tmp.err"
  programOut=$?
  diff "tmp.err" "${f%in}err" >/dev/null 2>&1 && diff "tmp.out" "${f%in}out" >/dev/null 2>&1
  testOut=$?
  filename=$(basename ${f%.in})
  if [ $testOut != 0 ] || [ $programOut == 32 ]
  then echo Dla testu: "$filename" niepowodzenie.
  else echo Dla testu: "$filename" powodzenie.
  fi
done
rm "tmp.out"
rm "tmp.err"
