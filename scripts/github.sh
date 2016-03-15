__dir__="$(cd "$(dirname "$0")" && pwd)"

# shellcheck source=scripts/digest.sh
source "$__dir__/digest.sh"

function rbx_github_release {
  local version date previous log body request response release_url upload_url

  version=$1
  date=$2

  release_url="https://api.github.com/repos/rubinius/rubinius/releases"

  IFS="." read -r -a array <<< "$version"
  let minor=${array[1]}-1

  if (( minor < 0 )); then
    previous=$version
  else
    previous="${array[0]}.$minor"
  fi

  log=$(git log --color=never --max-parents=1 --reverse --pretty='format:* %s (%an)%+b' \
    "v$previous..v$version" | sed 's/\[ci skip\]//' | sed 's/\[skip ci\]//' \
    | sed '/^$/N;/^\n$/D')

  unset GEM_HOME GEM_PATH GEM_ROOT

  body=$(printf 'Version %s (%s)\n\n%s' "$version" "$date" "$log" \
    | bin/rbx -r json -e "puts STDIN.read.to_json")

  request=$(printf '{
    "tag_name": "v%s",
    "target_commitish": "master",
    "name": "Release %s",
    "body": %s,
    "draft": false,
    "prerelease": false
  }' "$version" "$version" "$body")

  response=$(curl --data "$request" \
    "$release_url?access_token=$GITHUB_OAUTH_TOKEN")

  if [ $? -ne 0 ]; then
    echo "Creating GitHub release failed"
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

function rbx_github_update_file {
  local file sha message url branch content request

  file=$1
  sha=$2
  message=$3
  url=$4
  branch=${5:-master}
  content=$(rbx_base64 "$file")

  request=$(printf '{
    "message": "%s",
    "content": "%s",
    "sha": "%s",
    "branch": "%s"
  }' "$message" "$content" "$sha" "$branch")

  curl -X PUT --data "$request" "$url?access_token=$GITHUB_OAUTH_TOKEN"
}
