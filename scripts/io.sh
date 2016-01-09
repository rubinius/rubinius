function fail {
  local message=$1
  message=${message:=An unhandled error occurred}
  echo "$message at ${BASH_SOURCE[1]}:${FUNCNAME[1]} line ${BASH_LINENO[0]}." >&2
  exit 1
}
