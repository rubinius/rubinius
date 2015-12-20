#!/bin/bash

source "scripts/configuration.sh"
source "scripts/aws.sh"
source "scripts/io.sh"

function rbx_release_bucket {
  echo "rubinius-releases-rubinius-com"
}

function rbx_binary_bucket {
  echo "rubinius-binaries-rubinius-com"
}

function rbx_url_prefix {
  local bucket=$1
  echo "https://${bucket}.s3-us-west-2.amazonaws.com"
}

function rbx_upload_files {
  local bucket dest src path url name file_exts index

  bucket=$1
  dest=$2
  src=$3
  path=${4:-}
  url=$(rbx_url_prefix "$bucket")
  file_exts=("" ".sha512")
  index="index.txt"

  rbx_s3_download "$url" "$index"

  # Upload all the files first.
  for ext in "${file_exts[@]}"; do
    rbx_s3_upload "$url" "$bucket" "$dest$ext" "$src$ext" "$path" ||
      fail "unable to upload file"
  done

  # Update the index and upload it.
  for ext in "${file_exts[@]}"; do
    if [[ -n $path ]]; then
      name="$url$path$dest$ext"
    else
      name="$dest$ext"
    fi

    grep "$name" "$index"
    if [ $? -ne 0 ]; then
      echo "$name" >> "$index"
    fi
  done

  rbx_s3_upload "$url" "$bucket" "$index" "$index" || fail "unable to upload index"
}

# Build and upload the release tarball to S3.
if [[ $TRAVIS_OS_NAME == osx ]]; then
  echo "Deploying release tarball $(rbx_release_name)..."

  "$(rbx_script_path)/release.sh" || fail "unable to build release tarball"

  rbx_upload_files "$(rbx_release_bucket)" "$(rbx_release_name)" "$(rbx_release_name)"
fi

# Build and upload a Homebrew binary to S3.
if [[ $TRAVIS_OS_NAME == osx ]]; then
  echo "Deploying Homebrew binary $(rbx_release_name)..."

  "$(rbx_script_path)/package.sh" homebrew || fail "unable to build Homebrew binary"

  rbx_upload_files "$(rbx_binary_bucket)" "$(rbx_release_name)" \
    "$(rbx_release_name)" "/homebrew/"
fi

# Build and upload a binary to S3.
echo "Deploying Travis binary $(rbx_release_name) for ${TRAVIS_OS_NAME}..."

"$(rbx_scripts)/package.sh" binary || fail "unable to build binary"

declare -a paths os_releases versions

if [[ $TRAVIS_OS_NAME == linux ]]; then
  os_releases=("12.04" "14.04" "15.10")
  for (( i=0; i < ${#os_releases[@]}; i++ )); do
    paths[i]="/ubuntu/${os_releases[i]}/x86_64/"
  done
else
  os_releases=("10.9" "10.10" "10.11")
  for (( i=0; i < ${#os_releases[@]}; i++ )); do
    paths[i]="/osx/${os_releases[i]}/x86_64/"
  done
fi

IFS="." read -r -a array <<< "$(rbx_revision_version)"

let i=0
version=""
versions[i]=""

for v in "${array[@]}"; do
  let i=i+1
  versions[i]="-$version$v"
  version="$v."
done

for path in "${paths[@]}"; do
  for version in "${versions[@]}"; do
    rbx_upload_files "$(rbx_binary_bucket)" "rubinius$version.tar.bz2" \
      "$(rbx_release_name)" "$path"
  done
done
