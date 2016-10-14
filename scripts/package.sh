#!/bin/bash

__dir__="$(cd "$(dirname "$0")" && pwd)"

# shellcheck source=scripts/configuration.sh
source "$__dir__/configuration.sh"
# shellcheck source=scripts/digest.sh
source "$__dir__/digest.sh"

function rbx_package_tar {
  local archive files

  archive="$(rbx_release_name)"
  rm -rf "$archive"

  files="$(git ls-files; ls .revision; ls vendor/cache/*.gem)"

  echo "$files" | sort | uniq | \
    bsdtar -c -s "|^|rubinius-$(rbx_revision_version)/|" -T - -f - | bzip2 > "$archive"

  rbx_digest_file "$archive" "sha512"
}

function rbx_package_binary {
  rake package:binary
}

function rbx_package_homebrew {
  echo "Packaging for Homebrew..."

  rake package:homebrew

  rbx_digest_file "$(rbx_release_name)" "sha1"
  rbx_digest_file "$(rbx_release_name)" "sha256"
}

function rbx_package_heroku {
  echo "Packaging for Heroku..."

  rake package:heroku

  rbx_digest_file "$(rbx_heroku_release_name)" "sha1"
}

function rbx_package_core {
  echo "Packaging runtime/core..."

  rm -rf "$(rbx_runtime_core_name)"

  tar -c -f - runtime/core | bzip2 > "$(rbx_runtime_core_name)"

  rbx_digest_file "$(rbx_runtime_core_name)" "sha512"
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
    "heroku")
      rbx_package_heroku
      ;;
    "core")
      rbx_package_core
      ;;
    *)
      echo "Usage: ${0##*/} package_type"
      exit 1
      ;;
  esac
fi
