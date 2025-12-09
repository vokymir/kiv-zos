#!/bin/bash

# start from project root directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"/..

# Define the name of the output directory and the archive file
ARCHIVE_DIR="archive"
ARCHIVE_NAME="archive_$(date +%Y%m%d_%H%M%S).tar.gz"

# Ensure archive dir exist
mkdir -p "$ARCHIVE_DIR"

# list only tracked files
# & archive them using tar
git ls-files -z | xargs -0 tar -cvzf "$ARCHIVE_DIR"/"$ARCHIVE_NAME" -T -

echo "Created archive: ${ARCHIVE_DIR}/${ARCHIVE_NAME}"
