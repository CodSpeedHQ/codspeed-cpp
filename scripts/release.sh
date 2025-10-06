#!/bin/bash
set -e

# Check is on main
if [ "$(git rev-parse --abbrev-ref HEAD)" != "main" ]; then
    echo "Not on main branch"
    exit 1
fi

# First and only argument is the version number
VERSION=v$1 # The version number, prefixed with 'v'
# Prompt the release version
echo "Release version: $VERSION"
read -p "Are you sure you want to release this version? (y/n): " confirm
if [ "$confirm" != "y" ]; then
    echo "Aborting release"
    exit 1
fi

# Check that GITHUB_TOKEN is set
if [ -z "$GITHUB_TOKEN" ]; then
    echo "GITHUB_TOKEN is not set. Trying to fetch it from gh"
    GITHUB_TOKEN=$(gh auth token)

fi

git cliff -o CHANGELOG.md --tag $VERSION --github-token $GITHUB_TOKEN
git add CHANGELOG.md
git commit -m "chore: Release $VERSION"
git tag $VERSION -m "Release $VERSION"
git push origin main
git push origin $VERSION
gh release create $VERSION -t $VERSION --generate-notes -d
