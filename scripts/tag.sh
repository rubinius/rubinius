#!/bin/bash

__dir__="$(cd "$(dirname "$0")" && pwd)"

# shellcheck source=scripts/configuration.sh
source "$__dir__/configuration.sh"

function rbx_tag_release {
  git fetch --all -p

  IFS="." read -r -a array <<< "$(rbx_revision_version)"

  let major=${array[0]}
  let minor=${array[1]}+1

  version="${major}.${minor}"
  message="\"Version $version ($(date +"%F"))\""

  echo Tagging "v$version" as "$message"
  git tag -a -m "$message" "v$version"
  echo After a successful tag, push tags with 'git push origin master --tags'
}

function rbx_untag_release {
  local version untag_version

  version=$1

  if [[ -z $version ]]; then
    cat >&2 <<EOM
untag VERSION is required

EOM
    rbx_tag_usage
  fi

  git fetch --all -p

  IFS="." read -r -a array <<< "$(rbx_revision_version)"

  let major=${array[0]}
  let minor=${array[1]}

  untag_version="${major}.${minor}"

  git describe "v$untag_version"

  if [[ $? -eq 0 && "$version" == "$untag_version" ]]; then
    echo Untagging "v$version"
    git tag -d "v$version" && git push origin ":refs/tags/v$version"
  else
    echo NOT untagging "v$version", does not match "v$untag_version"
  fi
}

function rbx_tag_usage {
  cat >&2 <<-EOM
Usage: ${0##*/} option
  where option is
    tag              Tags a release at the current release + 1. Remember to "git push origin master --tags" afterward.
    untag VERSION    Untags VERSION if it is the current release
EOM
  exit 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
  if [[ -z $1 ]]; then
    rbx_tag_usage
  fi

  case "$1" in
    "tag")
      rbx_tag_release
      ;;
    "untag")
      rbx_untag_release "$2"
      ;;
    "-h"|"--help"|*)
      rbx_tag_usage
      ;;
  esac
fi
