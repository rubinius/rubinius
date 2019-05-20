#!/bin/bash

__dir__="$(cd "$(dirname "$0")" && pwd)"

# shellcheck source=scripts/configuration.sh
source "$__dir__/configuration.sh"
# shellcheck source=scripts/aws.sh
source "$__dir__/aws.sh"
# shellcheck source=scripts/io.sh
source "$__dir__/io.sh"

function rbx_archive_codedb {
  local archive ext url bucket

  archive="$(rbx_codedb_cache_name)"
  ext=".sha512"
  bucket="$(rbx_binary_bucket)"
  url=$(rbx_url_prefix "$bucket")
  path="/codedb/"

  rm -f "$archive" "$archive$ext"

  "$__dir__/package.sh" codedb || fail "unable to package codedb cache"

  rbx_s3_upload "$url" "$bucket" "$archive" "$archive" "$path" ||
    fail "unable to upload codedb cache archive"
  rbx_s3_upload "$url" "$bucket" "$archive$ext" "$archive$ext" "$path" ||
    fail "unable to upload codedb cache archive digest"
}

function rbx_build_support_usage {
  cat >&2 <<-EOM
Usage: ${0##*/} archive_core
EOM
  exit 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
  case "$1" in
    "archive_codedb")
      rbx_archive_codedb
      ;;
    "-h"|"--help"|*)
      rbx_build_support_usage
      ;;
  esac
fi
