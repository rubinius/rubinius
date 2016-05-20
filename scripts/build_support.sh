#!/bin/bash

__dir__="$(cd "$(dirname "$0")" && pwd)"

# shellcheck source=scripts/configuration.sh
source "$__dir__/configuration.sh"
# shellcheck source=scripts/aws.sh
source "$__dir__/aws.sh"
# shellcheck source=scripts/io.sh
source "$__dir__/io.sh"

function rbx_archive_core {
  local archive ext url bucket

  archive="$(rbx_runtime_core_name)"
  ext=".sha512"
  bucket="$(rbx_binary_bucket)"
  url=$(rbx_url_prefix "$bucket")

  rm -f "$archive" "$archive$ext"

  "$__dir__/package.sh" core || fail "unable to package runtime core"

  rbx_s3_upload "$url" "$bucket" "$archive" "$archive" "/runtime/" ||
    fail "unable to upload runtime core archive"
  rbx_s3_upload "$url" "$bucket" "$archive$ext" "$archive$ext" "/runtime/" ||
    fail "unable to upload runtime core archive digest"

  rm -f "$archive" "$archive$ext"
}

function rbx_build_support_usage {
  cat >&2 <<-EOM
Usage: ${0##*/} archive_core
EOM
  exit 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
  case "$1" in
    "archive_core")
      rbx_archive_core
      ;;
    "-h"|"--help"|*)
      rbx_build_support_usage
      ;;
  esac
fi
