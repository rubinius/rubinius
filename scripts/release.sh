#!/bin/bash

__dir__="$(cd $(dirname "$0") && pwd)"

source "$__dir__/configuration.sh"

rbx_write_revision_file
"$__dir__/package.sh" tar
