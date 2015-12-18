function rbx_digest_file {
  local name digest

  name=$1
  digest=${2:-"sha512"}

  openssl dgst -"$digest" -hex "$name" | cut -d ' ' -f 2 > "$name.$digest"
}

function rbx_package_tar {
  local archive files prefix

  archive="$(rbx_release_name)"
  rm -rf "$archive"

  files="$(git ls-files; ls .revision; ls vendor/cache/*.gem)"

  echo "$files" | sort | uniq | \
    tar -c -s "|^|rubinius-$(rbx_revision_version)/|" -T - -f - | bzip2 > "$archive"

  rbx_digest_file "$archive" "sha512"
}
