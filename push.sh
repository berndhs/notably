#!/bin/bash

for R in central github sf assembla
do
  echo "push to ${R}"
  git push ${R} master
done
