# Redefine STDIN, STDOUT & STDERR to use the new IO class. It reopened and redefined
# all methods used in the bootstrap step.

def redefine_io(const)
  descriptor = const.descriptor
  mode = const.mode
  new_io = IO.for_fd(descriptor, mode, Hash.new)
  new_io.mode = mode
  return new_io
end

STDIN  = redefine_io(STDIN)
STDOUT = redefine_io(STDOUT)
STDERR = redefine_io(STDERR)