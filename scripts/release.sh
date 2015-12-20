#!/bin/bash

source "scripts/configuration.sh"

rbx_write_revision_file

"$(rbx_script_path)/package.sh" tar
