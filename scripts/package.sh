#!/bin/bash

__dir__="$(cd "$(dirname "$0")" && pwd)"

# shellcheck source=scripts/configuration.sh
source "$__dir__/configuration.sh"

function rbx_digest_file {
  local name digest

  name=$1
  digest=${2:-"sha512"}

  openssl dgst -"$digest" -hex "$name" | cut -d ' ' -f 2 > "$name.$digest"
}

function rbx_package_tar {
  local archive files

  archive="$(rbx_release_name)"
  rm -rf "$archive"

  files="$(git ls-files; ls .revision; ls vendor/cache/*.gem)"

  echo "$files" | sort | uniq | \
    tar -c -s "|^|rubinius-$(rbx_revision_version)/|" -T - -f - | bzip2 > "$archive"

  rbx_digest_file "$archive" "sha512"
}

function rbx_package_binary {
  rake package:binary
}

function rbx_package_homebrew {
  echo "Packaging for Homebrew..."
  rake package:homebrew
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
  case "$1" in
    "tar")
      rbx_package_tar
      ;;
    "binary")
      rbx_package_binary
      ;;
    "homebrew")
      rbx_package_homebrew
      ;;
    *)
      echo "Usage: ${0##*/} package_type"
      exit 1
      ;;
  esac
fi
