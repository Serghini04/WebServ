#!/bin/bash

if [$1 == '']; then
	echo "Please provide a branch name"
	exit 1
fi
git checkout $1
git pull origin master
git fetch origin
git merge origin/main
git push origin $1