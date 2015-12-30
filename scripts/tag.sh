#!/bin/bash

__dir__="$(cd "$(dirname "$0")" && pwd)"

# shellcheck source=scripts/configuration.sh
source "$__dir__/configuration.sh"

IFS="." read -r -a array <<< "$(rbx_revision_version)"

let major=${array[0]}
let minor=${array[1]}+1

version="${major}.${minor}"
message="\"Version $version ($(date +"%F"))\""

echo Tagging "v$version" as "$message"
git tag -a -m "$message" "v$version"
