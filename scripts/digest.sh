function rbx_digest_file {
  local name digest

  name=$1
  digest=${2:-"sha512"}

  openssl dgst -"$digest" -hex "$name" | cut -d ' ' -f 2 > "$name.$digest"
}

function rbx_base64 {
  base64 -i "$1" | tr -d '\n'
}
