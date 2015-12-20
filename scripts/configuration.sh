function rbx_script_path {
  local dir
  dir=$(dirname "$0")

  cd "$dir" && pwd
}

function rbx_git_directory {
  echo "$(rbx_script_path)/../.git"
}

function rbx_revision_file {
  echo "$(rbx_script_path)/../.revision"
}

function rbx_get_revision {
  local describe count version commit_count commit_date commit_hash

  if [[ -d $(rbx_git_directory) ]]; then
    describe=$(git describe --tags --match=v2* --abbrev=40 --long)

    count=$(echo "$describe" | cut -d - -f 2)
    if [[ $count != "0" ]]; then
      commit_count=".c$count"
    fi

    version=$(echo "$describe" | egrep -o '^v\d+\.\d+' | cut -c 2-)
    commit_date=$(git show --format="%ci" HEAD | egrep -o '\d+-\d+-\d+')
    commit_hash=$(echo "$describe" | egrep -o '[[:xdigit:]]{8,}$')

    echo "$version$commit_count $commit_date $commit_hash"
  elif [[ -f $(rbx_revision_file) ]]; then
    cat "$(rbx_revision_file)"
  else
    echo "X.Y $( date +"%F" ) build"
  fi
}

function rbx_revision_version {
  rbx_get_revision | cut -d ' ' -f 1
}

function rbx_revision_date {
  rbx_get_revision | cut -d ' ' -f 2
}

function rbx_revision_hash {
  rbx_get_revision | cut -d ' ' -f 3
}

function rbx_write_revision_file {
  if [[ -d $(rbx_git_directory) ]]; then
    rbx_get_revision > "$(rbx_revision_file)"
  fi
}

function rbx_release_name {
  echo "rubinius-$(rbx_revision_version).tar.bz2"
}
