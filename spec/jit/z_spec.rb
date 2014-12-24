# TODO: Implement JIT spec infrastructure to make this hack unnecessary.
#
# During a complete spec run, this file will be loaded last and reset the JIT
# to not be in sync mode. When running a single spec file in spec/jit, the
# process will exit after running the specs so leaving the JIT in sync mode
# will not have any consequences.

Rubinius::JIT.sync = RubiniusJITSync
