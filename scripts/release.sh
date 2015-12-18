#!/bin/bash

source "scripts/configuration.sh"
source "scripts/package.sh"

function rbx_release_name {
  echo "rubinius-$(rbx_revision_version).tar.bz2"
}

rbx_write_revision_file
rbx_package_tar

exit 1
