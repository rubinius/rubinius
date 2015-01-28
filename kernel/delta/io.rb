# Redefine STDIN, STDOUT & STDERR to use the new IO class. It reopened and redefined
# all methods used in the bootstrap step. Secondly, update the $std* globals to point
# to the new objects.

def redefine_io(fd, mode)
  # Note that we use IO.open instead of IO.reopen. The reason is that we reopened the
  # IO class in common/io.rb and overwrote a lot of the methods that were defined in
  # bootstrap/io.rb. So if we try to use IO.reopen on the original IO object, it won't
  # be able to reach those original methods anymore. So, we just pass in the file
  # descriptor integer directly and wrap it up in a new object. The original object
  # will probably get garbage collected but we don't set a finalizer for FDs 0-2 which
  # correspond to STDIN, STDOUT and STDERR so we don't need to worry that they'll get
  # closed out from under us.
  # Hopefully we can find a cleaner way to do this in the future, but for now it's a 
  # bit ugly.
  new_io = IO.open(fd)
  new_io.sync = true
  
  if mode == :read_only
    new_io.force_read_only
  elsif mode == :write_only
    new_io.force_write_only
  end
  
  return new_io
end

STDIN  = redefine_io(0, :read_only)
STDOUT = redefine_io(1, :write_only)
STDERR = redefine_io(2, :write_only)

Rubinius::Globals.set!(:$stdin, STDIN)
Rubinius::Globals.set!(:$stdout, STDOUT)
Rubinius::Globals.set!(:$stderr, STDERR)
