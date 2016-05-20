#!/bin/bash

__dir__="$(cd "$(dirname "$0")" && pwd)"

# shellcheck source=scripts/configuration.sh
source "$__dir__/configuration.sh"
# shellcheck source=scripts/digest.sh
source "$__dir__/digest.sh"
# shellcheck source=scripts/aws.sh
source "$__dir__/aws.sh"
# shellcheck source=scripts/io.sh
source "$__dir__/io.sh"

function rbx_archive_core {
  local archive ext path bucket url

  archive="$(rbx_runtime_core_name)"
  ext=".sha512"
  path="$(rbx_runtime_core_path)"
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

function rbx_fetch_core {
  local archive digest check path bucket url

  archive="$(rbx_runtime_core_name)"
  digest="$archive.sha512"
  check="$digest.check"
  path="$(rbx_runtime_core_path)"
  bucket="$(rbx_binary_bucket)"
  url=$(rbx_url_prefix "$bucket")

  rbx_s3_download "$url" "$archive" "$path"
  rbx_s3_download "$url" "$digest" "$path"

  mv "$digest" "$check"

  rbx_digest_file "$archive"

  diff "$digest" "$check"
  if [ $? -ne 0 ]; then
    fail "runtime core achive digest does not match"
  fi

  rm -f "$digest" "$check"
}

function rbx_setup_core {
  local runtime

  runtime="./runtime"

  rm -rf "$runtime/core"

  if [ ! -d "$runtime" ]; then mkdir "$runtime"; fi

  bzip2 -dc "$(rbx_runtime_core_name)" | tar -x -f -
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
    "fetch_core")
      rbx_fetch_core
      ;;
    "setup_core")
      rbx_setup_core
      ;;
    "-h"|"--help"|*)
      rbx_build_support_usage
      ;;
  esac
fi
