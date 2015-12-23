__dir__="$(cd "$(dirname "$0")" && pwd)"

function rbx_github_release_url {
  echo "https://api.github.com/repos/rubinius/rubinius/releases"
}

function rbx_github_release {
  local version date request response upload_url

  version=$1
  date=$2

  request=$(printf '{
    "tag_name": "v%s",
    "target_commitish": "master",
    "name": "Release %s",
    "body": "Version %s (%s)",
    "draft": false,
    "prerelease": false
  }' "$version" "$version" "$version" "$date")

  response=$(curl --data "$request" \
    "$(rbx_github_release_url)?access_token=$GITHUB_OAUTH_TOKEN")

  if [ $? -ne 0 ]; then
    return
  fi

  upload_url=$(echo "$response" | "$__dir__/json.sh" -b | \
    egrep '\["upload_url"\][[:space:]]\"[^"]+\"' | egrep -o '\"[^"]+{')

  let i=${#upload_url}

  echo "${upload_url:1:$i-2}"
}

function rbx_github_release_assets {
  local upload_url name src file_exts

  upload_url=$1
  name=$2

  file_exts=("" ".sha512")

  for ext in "${file_exts[@]}"; do
    src="$name$ext"

    curl -X PUT -T "$src" \
      -H "Content-Type: $(file --mime-type -b "$src")" \
      "$upload_url?name=$src&access_token=$GITHUB_OAUTH_TOKEN"
  done
}
