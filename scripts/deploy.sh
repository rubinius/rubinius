#!/bin/bash

source "scripts/io.sh"
source "scripts/aws.sh"
source "scripts/configuration.sh"

release_name="rubinius-$(rbx_revision_version).tar.bz2"

file_exts=("" ".md5" ".sha1" ".sha512")

function rbx_url_prefix {
  local bucket=$1
  echo "https://${bucket}.s3-us-west-2.amazonaws.com"
}

function rbx_upload_files {
  local bucket file path url name

  bucket=$1
  file=$2
  path=${3:-}
  url=$(rbx_url_prefix "$bucket")

  rbx_s3_download "$url" "index.txt"

  # Upload all the files first.
  for ext in "${file_exts[@]}"; do
    rbx_s3_upload "$url" "$bucket" "$file$ext" "$path" || fail "unable to upload file"
  done

  # Update the index and upload it.
  for ext in "${file_exts[@]}"; do
    if [[ -n $path ]]; then
      name="$url$path$file$ext"
    else
      name="$file$ext"
    fi

    grep "$name" "index.txt"
    if [ $? -ne 0 ]; then
      echo "$name" >> "index.txt"
    fi
  done

  rbx_s3_upload "$url" "$bucket" "index.txt" || fail "unable to upload index"
}

# Build and upload the release tarball to S3.
if [[ $TRAVIS_OS_NAME == linux && $CC == clang && $RVM == "rbx-2" ]]; then
  echo "Deploying release tarball ${release_name}..."

  rake release || fail "unable to build release tarball"

  bucket="rubinius-releases-rubinius-com"

  rbx_upload_files "$bucket" "$release_name"
fi

# Build and upload a binary to S3.
if [[ $RVM == "rbx-2" ]]; then
  echo "Deploying Travis binary ${release_name} for ${TRAVIS_OS_NAME}..."

  rake package:binary || fail "unable to build binary"

  bucket="rubinius-binaries-rubinius-com"

  if [[ $TRAVIS_OS_NAME == linux ]]; then
    path="ubuntu/12.04/x86_64/"
  else
    path="osx/10.9/x86_64/"
  fi

  rbx_upload_files "$bucket" "$release_name" "$path"
fi
