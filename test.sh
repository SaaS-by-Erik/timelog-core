#!/usr/bin/env bash

n_tests=0
n_tests_failed=0
n_tests_passed=0

tempdir=$( mktemp -d )

if [ $? -ne 0 ] ; then
  echo "Failed to create temporary directory for tests." 1>&2
  exit 1
fi

origdir=$( pwd )
cd "$tempdir"

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl init 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: \`tl init'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl init 2>/dev/null
if [ $? -eq 0 ] ; then
  echo "Test: \`tl init' where \`tl init' has been done. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
if [ ! -f tl.db ] ; then
  echo "Test: File \`tl.db' created. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
if [ ! -f stack.db ] ; then
  echo "Test: File \`stack.db' created. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl 2>/dev/null
if [ $? -eq 0 ] ; then
  echo "Test: Run \`tl' without arguments. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
$origdir/bin/tl x 2>/dev/null
if [ $? -eq 0 ] ; then
  echo "Test: \`tl x' -- Invalid command. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
rm stack.db 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: Remove \`stack.db'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

n_tests=$(( $n_tests + 1 ))
rm tl.db 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Test: Remove \`tl.db'. Failed." 1>&2
  n_tests_failed=$(( $n_tests_failed + 1 ))
else
  n_tests_passed=$(( $n_tests_passed + 1 ))
fi

cd -
rmdir "$tempdir"

echo "Ran $n_tests tests. $n_tests_passed passed. $n_tests_failed failed."

if [ $n_tests_passed -lt $n_tests ] ; then
  exit 1
fi
