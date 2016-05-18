#!/bin/bash

LESSBIN=`which less`

if [ "$LESSBIN" = "" ]
then
  echo "You need to have 'less' program installed";
  exit 1;
fi

$LESSBIN $1

echo "Please type \"yes\" if you accept the terms of the license"
read -e ANSWER

if [ "$ANSWER" = "yes" ]
then
  exit 0;
fi

echo "Aborting installation"
exit 2;

