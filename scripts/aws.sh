# Adapted from https://gist.github.com/chrismdp/6c6b6c825b07f680e710
function rbx_s3_upload {
  local url bucket path dest src date acl content_type data signature

  url=$1
  bucket=$2
  dest=$3
  src=$4
  path=${5:-/}

  date=$(date +"%a, %d %b %Y %T %z")

  acl="x-amz-acl:public-read"
  content_type=$(file --mime-type -b "$src")

  data="PUT\n\n$content_type\n$date\n$acl\n/$bucket$path$dest"
  signature=$(echo -en "${data}" |
              openssl sha1 -hmac "${AWS_SECRET_ACCESS_KEY}" -binary | base64)

  curl -X PUT -T "$src" \
    -H "Host: $bucket.s3-us-west-2.amazonaws.com" \
    -H "Date: $date" \
    -H "Content-Type: $content_type" \
    -H "$acl" \
    -H "Authorization: AWS ${AWS_ACCESS_KEY_ID}:$signature" \
    --verbose \
    "$url$path$dest"
}

function rbx_s3_download {
  local url file path

  url=$1
  file=$2
  path=${3:-/}

  curl -o "$file" "$url$path$file"
}
