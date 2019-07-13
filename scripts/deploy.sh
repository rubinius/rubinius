#!/bin/bash

__dir__="$(cd "$(dirname "$0")" && pwd)"

# shellcheck source=scripts/configuration.sh
source "$__dir__/configuration.sh"
# shellcheck source=scripts/digest.sh
source "$__dir__/digest.sh"
# shellcheck source=scripts/github.sh
source "$__dir__/github.sh"
# shellcheck source=scripts/aws.sh
source "$__dir__/aws.sh"
# shellcheck source=scripts/io.sh
source "$__dir__/io.sh"

function rbx_upload_files {
  local bucket dest src path url name index
  local -a file_exts

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
    if [[ -f $src$ext ]]; then
      rbx_s3_upload "$url" "$bucket" "$dest$ext" "$src$ext" "$path" ||
        fail "unable to upload file"
    fi
  done

  # Update the index and upload it.
  for ext in "${file_exts[@]}"; do
    if [[ -n $path ]]; then
      name="$url$path$dest$ext"
    else
      name="$dest$ext"
    fi

    grep "^$name\$" "$index"
    if [ $? -ne 0 ]; then
      echo "$name" >> "$index"
    fi
  done

  rbx_s3_upload "$url" "$bucket" "$index" "$index" || fail "unable to upload index"
}

# Build and upload the release tarball to S3.
function rbx_deploy_release_tarball {
  if [[ $1 == linux ]]; then
    echo "Deploying release tarball $(rbx_release_name)..."

    "$__dir__/release.sh" || fail "unable to build release tarball"

    rbx_upload_files "$(rbx_release_bucket)" "$(rbx_release_name)" "$(rbx_release_name)"
  fi
}

# Build and upload a Homebrew binary to S3.
function rbx_deploy_homebrew_binary {
  if [[ $1 != osx ]]; then
    return
  fi

  echo "Deploying Homebrew binary $(rbx_release_name)..."

  "$__dir__/package.sh" homebrew || fail "unable to build Homebrew binary"

  rbx_upload_files "$(rbx_binary_bucket)" "$(rbx_release_name)" \
    "$(rbx_release_name)" "/homebrew/"

  echo "Deploying Homebrew release $(rbx_revision_version)..."

  local release file url response sha

  release=$(rbx_release_name)
  file="rubinius.rb"
  url="https://api.github.com/repos/rubinius/homebrew-apps/contents/$file"
  response=$(curl "$url?access_token=$GITHUB_OAUTH_TOKEN")

  cat > "$file" <<EOF
require 'formula'

class Rubinius < Formula
  homepage 'http://rubinius.com/'
  url 'https://rubinius-binaries-rubinius-com.s3.amazonaws.com/homebrew/$release'
  sha256 '$(cat "$release.sha256")'

  depends_on 'libyaml'

  depends_on :arch => :x86_64
  depends_on MinimumMacOSRequirement => :mountain_lion

  keg_only "Conflicts with MRI (Matz's Ruby Implementation)."

  def install
    bin.install Dir["bin/*"]
    lib.install Dir["lib/*"]
    include.install Dir["include/*"]
    man1.install Dir["man/man1/*"]
  end

  test do
    assert_equal 'rbx', \`"#{bin}/rbx" -e "puts RUBY_ENGINE"\`.chomp
  end
end
EOF

  sha=$(echo "$response" | "$__dir__/json.sh" -b | \
    egrep '\["sha"\][[:space:]]\"[^"]+\"' | egrep -o '\"[[:xdigit:]]+\"')

  let i=${#sha}

  rbx_github_update_file "$file" "${sha:1:$i-2}" "Version $(rbx_revision_version)" "$url"

  rm "$file"
}

# Build and upload a Heroku binary to S3.
function rbx_deploy_heroku_binary {
  if [[ $1 != linux ]]; then
    return
  fi

  if [[ $(rbx_dist_version) != "14.04" ]]; then
    return
  fi

  echo "Deploying Heroku binary $(rbx_heroku_release_name)..."

  "$__dir__/package.sh" heroku || fail "unable to build Heroku binary"

  local -a file_exts=("" ".sha1")
  local url bucket name

  bucket="$(rbx_binary_bucket)"
  url="$(rbx_url_prefix "$bucket")"
  name="$(rbx_heroku_release_name)"

  for ext in "${file_exts[@]}"; do
    if [[ -f $name$ext ]]; then
      rbx_s3_upload "$url" "$bucket" "$name$ext" "$name$ext" "/heroku/cedar-14/" ||
        fail "unable to upload file"
    fi
  done
}

# Build and upload a binary to S3.
function rbx_deploy_travis_binary {
  local os_name vendor_llvm

  os_name=$1
  echo "Deploying Travis binary $(rbx_release_name) for $os_name..."

  if [[ $os_name == linux ]]; then
    export RBX_BINARY_CONFIG='--llvm-config=llvm-config --cc=clang --cxx=clang++'
  elif [[ $os_name == osx ]]; then
    export RBX_BINARY_CONFIG='--with-include-dir=$OPENSSL_DIR/include --with-lib-dir=$OPENSSL_DIR/lib'
  fi

  "$__dir__/package.sh" binary || fail "unable to build binary"

  declare -a paths os_releases versions

  if [[ $os_name == linux ]]; then
    os_releases=($(rbx_dist_version))
    for (( i=0; i < ${#os_releases[@]}; i++ )); do
      paths[i]="/ubuntu/${os_releases[i]}/x86_64/"
    done
  elif [[ $os_name == osx ]]; then
    os_releases=("10.9" "10.10" "10.11")
    for (( i=0; i < ${#os_releases[@]}; i++ )); do
      paths[i]="/osx/${os_releases[i]}/x86_64/"
    done
  else
    echo "${FUNCNAME[1]}: no match for OS name"
    return
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
}

function rbx_trigger_deploy {
  if [[ $1 != linux ]]; then
    return
  fi

  local branch json_url response sha raw_url travis_yml version

  branch=$2
  json_url="https://api.github.com/repos/rubinius/rubinius-build/contents/.travis.yml"

  response=$(curl "$json_url?ref=$branch&access_token=$GITHUB_OAUTH_TOKEN")
  sha=$(echo "$response" | "$__dir__/json.sh" -b | \
    egrep '\["sha"\][[:space:]]\"[^"]+\"' | egrep -o '\"[[:xdigit:]]+\"')

  raw_url="https://raw.githubusercontent.com/rubinius/rubinius-build/$branch/.travis.yml"
  response=$(curl "$raw_url?access_token=$GITHUB_OAUTH_TOKEN")

  travis_yml=rubinius-build.travis.yml
  version=$(rbx_revision_version)

  echo "$response" | \
    sed "s/RUBINIUS_VERSION=.*$/RUBINIUS_VERSION=$version/" \
    > "$travis_yml"

  rbx_github_update_file "$travis_yml" "${sha:1:${#sha}-2}" \
    "Version $version." "$json_url" "$branch"

  rm -f "$travis_yml"
}

function rbx_deploy_github_release {
  local upload_url

  if [[ $1 == linux ]]; then
    upload_url=$(rbx_github_release "$(rbx_revision_version)" "$(rbx_revision_date)")

    rbx_github_release_assets "$upload_url" "$(rbx_release_name)"
  fi
}

function rbx_deploy_website_release {
  if [[ $1 != linux ]]; then
    return
  fi

  local releases updates version url response sha download_url

  releases="releases.yml"
  updates="updated_$releases"
  version=$(rbx_revision_version)
  url="https://api.github.com/repos/rubinius/rubinius.com/contents/source/_data/releases.yml"

  response=$(curl "$url?access_token=$GITHUB_OAUTH_TOKEN")

  download_url=$(echo "$response" | "$__dir__/json.sh" -b | \
    egrep '\["download_url"\][[:space:]]\"[^"]+\"' | egrep -o '\"[^\"]+\"$')
  curl -o "$releases" "${download_url:1:${#download_url}-2}?access_token=$GITHUB_OAUTH_TOKEN"

  grep "^- version: \"$version\"\$" "$releases"
  if [ $? -eq 0 ]; then
    return
  fi

  sha=$(echo "$response" | "$__dir__/json.sh" -b | \
    egrep '\["sha"\][[:space:]]\"[^"]+\"' | egrep -o '\"[[:xdigit:]]+\"')

  cat > "$updates" <<EOF
- version: "$version"
  date: $(rbx_revision_date)
EOF

  cat "$releases" >> "$updates"

  rbx_github_update_file "$updates" "${sha:1:${#sha}-2}" "Version $version." "$url"
}

function rbx_deploy_docker_release {
  if [[ $1 != linux ]]; then
    return
  fi

  echo "Deploying Docker release $(rbx_revision_version)..."

  local version release path file url response sha

  version=$(rbx_revision_version)
  release=$(rbx_release_name)
  path=$(rbx_dist_version)
  file="Dockerfile"

  url="https://api.github.com/repos/rubinius/docker/contents/ubuntu/$path/$file"
  response=$(curl "$url?access_token=$GITHUB_OAUTH_TOKEN")

  cat > "$file" <<EOF
FROM ubuntu:$path

RUN locale-gen en_US.UTF-8
RUN update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 LANGUAGE=en_US.UTF-8

ENV LC_ALL en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US.UTF-8

RUN apt-get update && apt-get install -y \\
        bzip2 \\
        libyaml-0-2 \\
        libssl1.0.0 \\
        clang-3.6 \\
        libedit-dev \\
        zlib1g-dev \\
        make

ADD https://rubinius-binaries-rubinius-com.s3-us-west-2.amazonaws.com/ubuntu/$path/x86_64/$release /tmp/rubinius.tar.bz2
RUN cd /opt && tar xvjf /tmp/rubinius.tar.bz2

RUN ln -sf `which clang-3.6` /usr/bin/cc
RUN ln -sf `which clang++-3.6` /usr/bin/c++

ENV PATH /opt/rubinius/$version/bin:/opt/rubinius/$version/gems/bin:\$PATH

CMD ["bash"]
EOF

  sha=$(echo "$response" | "$__dir__/json.sh" -b | \
    egrep '\["sha"\][[:space:]]\"[^"]+\"' | egrep -o '\"[[:xdigit:]]+\"')

  rbx_github_update_file "$file" "${sha:1:${#sha}-2}" \
    "Updated Dockerfile for $path to $version.\n\n[ci skip]" "$url"

  rm "$file"
}

function rbx_deploy_usage {
  cat >&2 <<-EOM
Usage: ${0##*/} [release github travis heroku homebrew website docker trigger-heroku trigger-homebrew trigger-travis-osx trigger-travis-14.04 trigger-travis-16.04 triggers]
EOM
  exit 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
  if [[ -z $1 ]]; then
    rbx_deploy_usage
  fi

  for cmd in "${@}"; do
    case "$cmd" in
      "release")
        rbx_deploy_release_tarball "$TRAVIS_OS_NAME"
        ;;
      "github")
        rbx_deploy_github_release "$TRAVIS_OS_NAME"
        ;;
      "travis")
        rbx_deploy_travis_binary "$TRAVIS_OS_NAME"
        ;;
      "heroku")
        rbx_deploy_heroku_binary "$TRAVIS_OS_NAME"
        ;;
      "homebrew")
        rbx_deploy_homebrew_binary "$TRAVIS_OS_NAME"
        ;;
      "website")
        rbx_deploy_website_release "$TRAVIS_OS_NAME"
        ;;
      "docker")
        rbx_deploy_docker_release "$TRAVIS_OS_NAME"
        ;;
      "trigger-heroku")
        rbx_trigger_deploy "$TRAVIS_OS_NAME" heroku
        ;;
      "trigger-homebrew")
        rbx_trigger_deploy "$TRAVIS_OS_NAME" homebrew
        ;;
      "trigger-travis-osx")
        rbx_trigger_deploy "$TRAVIS_OS_NAME" travis-osx
        ;;
      "trigger-travis-14.04")
        rbx_trigger_deploy "$TRAVIS_OS_NAME" travis-14.04
        ;;
      "trigger-travis-16.04")
        rbx_trigger_deploy "$TRAVIS_OS_NAME" travis-16.04
        ;;
      "triggers")
        rbx_trigger_deploy "$TRAVIS_OS_NAME" heroku
        rbx_trigger_deploy "$TRAVIS_OS_NAME" homebrew
        rbx_trigger_deploy "$TRAVIS_OS_NAME" travis-osx
        rbx_trigger_deploy "$TRAVIS_OS_NAME" travis-14.04
        rbx_trigger_deploy "$TRAVIS_OS_NAME" travis-16.04
        ;;
      "-h"|"--help"|*)
        rbx_deploy_usage
        ;;
    esac
  done
fi
