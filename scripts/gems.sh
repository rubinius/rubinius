#!/bin/bash

function rbx_update_gems_list {
  local gems_list build_list gemfile

  gems_list=gems_list.txt
  build_list=build_gems_list.txt

  rm -f "$build_list"

  for gemfile in Gemfile Gemfile.installed; do
    export BUNDLE_GEMFILE=$gemfile

    bundle update && bundle package --no-prune && bundle list >> "$build_list"
  done

  unset BUNDLE_GEMFILE

  sort < "$build_list" | uniq | \
    sed 's/^.*\*[[:space:]]*\(.*\)[[:space:]](\(.*\))$/\1-\2.gem/' | \
    egrep '\.gem$' > "$gems_list"

  rm -f "$build_list"
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
  case "$1" in
    "update_list")
      rbx_update_gems_list
      ;;
    *)
      echo "Usage: ${0##*/} [update_list]"
      exit 1
      ;;
  esac
fi
