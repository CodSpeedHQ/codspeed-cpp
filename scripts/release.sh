#!/bin/bash
set -e

# First and only argument is the version number
VERSION_NO_V=${1} # The version number without the 'v' prefix
VERSION=v$1 # The version number, prefixed with 'v'

# Validate version format (X.Y.Z or X.Y.Z-alpha where X, Y, Z are integers)
if [[ ! "$VERSION_NO_V" =~ ^[0-9]+\.[0-9]+\.[0-9]+(-alpha)?$ ]]; then
    echo "Error: Invalid version format '$VERSION_NO_V'"
    echo "Usage: $0 <version>"
    echo "  Version must be in format X.Y.Z or X.Y.Z-alpha (e.g., 1.2.3 or 1.2.3-alpha)"
    exit 1
fi

# Check is on main (unless releasing an alpha version)
if [[ ! "$VERSION_NO_V" =~ -alpha ]]; then
    if [ "$(git rev-parse --abbrev-ref HEAD)" != "main" ]; then
        echo "Not on main branch (only alpha releases can be made from non-main branches)"
        exit 1
    fi
fi

# Check that GITHUB_TOKEN is set
if [ -z "$GITHUB_TOKEN" ]; then
    echo "GITHUB_TOKEN is not set. Trying to fetch it from gh"
    GITHUB_TOKEN=$(gh auth token)
fi

# List of files and line numbers to update with version numbers
# Format: "file:line_number"
VERSION_FILES=(
    "core/CMakeLists.txt:3"
    "core/MODULE.bazel:3"
    "google_benchmark/MODULE.bazel:3"
    "google_benchmark/MODULE.bazel:6"
)

# Get current version from core/CMakeLists.txt
PREVIOUS_VERSION=$(awk -F'[ )]' '/set\(CODSPEED_VERSION/ {print $2}' core/CMakeLists.txt)

# Prompt the release version
echo "Previous version: ${PREVIOUS_VERSION}"
echo "New version:      ${VERSION_NO_V}"
read -p "Are you sure you want to release this version? (y/n): " confirm
if [ "$confirm" != "y" ]; then
    echo "Aborting release"
    exit 1
fi

# Update version in all relevant files
echo "Updating version numbers in source files..."

for entry in "${VERSION_FILES[@]}"; do
    IFS=':' read -r file line_num <<< "$entry"
    # Use sed in a cross-platform way (macOS requires empty string after -i)
    if [[ "$OSTYPE" == "darwin"* ]]; then
        sed -i '' "${line_num}s/${PREVIOUS_VERSION}/${VERSION_NO_V}/" "$file"
    else
        sed -i "${line_num}s/${PREVIOUS_VERSION}/${VERSION_NO_V}/" "$file"
    fi
    echo "  Updated $file:$line_num"
done

# Commit version changes
printf "%s\n" "${VERSION_FILES[@]%%:*}" | sort -u | xargs git add

git cliff -o CHANGELOG.md --tag "$VERSION" --github-token "$GITHUB_TOKEN"
git add CHANGELOG.md
git commit -m "chore: Release $VERSION"
git tag -s "$VERSION" -m "Release $VERSION"
git push origin main
git push origin "$VERSION"

# Create tarball with submodules included
git submodule update --init --recursive
echo "Creating release tarball with submodules..."
TMPDIR=$(mktemp -d)
ARCHIVE_NAME="codspeed-cpp-${VERSION}"
TARBALL_NAME="${ARCHIVE_NAME}.tar.gz"

# Create main archive
git archive --prefix="${ARCHIVE_NAME}/" --format=tar HEAD | \
  (cd "$TMPDIR" && tar xf -)

# Add submodule content
git submodule foreach --recursive "git archive --prefix=${ARCHIVE_NAME}/\$path/ --format=tar HEAD | (cd $TMPDIR && tar xf -)"

# Create final tarball
(cd "$TMPDIR" && tar czf "$TMPDIR/$TARBALL_NAME" "$ARCHIVE_NAME")

echo "Tarball created at: $TMPDIR/$TARBALL_NAME"

# Create GitHub release with the tarball
if [[ "$VERSION_NO_V" =~ -alpha ]]; then
    gh release create "$VERSION" -t "$VERSION" --generate-notes --latest=false "$TMPDIR/$TARBALL_NAME"
else
    gh release create "$VERSION" -t "$VERSION" --generate-notes --latest "$TMPDIR/$TARBALL_NAME"
fi

# Cleanup
rm -rf "$TMPDIR"
