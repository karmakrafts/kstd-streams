#!/bin/bash

# Allows to easily update the CMake submodule in one quick swoop~
# @author Alexander Hinze
# @since 06/08/2023

if [ "$1" = "push" ]; then
	export PUSH_DIRECTLY=true
fi

export PREV_BRANCH=$(git rev-parse --abbrev-ref HEAD)

if [ `git rev-parse --verify develop 2>/dev/null` ]; then
	export HAS_DEVELOP=true
	git checkout develop
fi

cd cmake
git pull
cd ..
git add -A
git commit -m "Update CMake submodule to latest commit"

if [ "$PUSH_DIRECTLY" = true ]; then
	git push
fi

if [ "$HAS_DEVELOP" = true ]; then
	git checkout master
	git merge develop
	if [ "$PUSH_DIRECTLY" = true ]; then
		git push
	fi
fi

git checkout $PREV_BRANCH
