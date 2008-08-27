##
# Implements the MVM functionality.

class Rubinius::VM
  def self.spawn(*args)
    args.unshift "rubinius"
    ret = spawn_prim(args)
    return new(*ret)
  end

  def self.get_message
    # This is how we go to sleep until someone sends us something
    # MESSAGE_IO is a pipe that the environment will send a magic
    # byte down to tell us that there is a message read.
    Rubinius::MESSAGE_IO.sysread(1)
    poll_message
  end

  def self.each_message
    while true
      yield get_message
    end
  end

  def initialize(id, stdin, stdout, stderr)
    @id = id
    @stdin = stdin
    @stdout = stdout
    @stderr = stderr

    @stdin.setup
    @stdout.setup
  end

  attr_reader :id
  attr_reader :stdin
  attr_reader :stdout
  attr_reader :stderr

  def join
    self.class.join @id
  end

  def <<(obj)
    self.class.send_message @id, obj
  end

  ##
  # Sets a default VM debug channel to be used for handling yield_debugger
  # bytecodes. The caller must ensure that a debugger thread is waiting on the
  # receive end of the debug channel before any yield_debugger ops are hit.
  #
  # If no VM debug channel is specified, individual threads must have
  # #set_debugging called on them before they encounter a yield_debugger op.
  #
  # Only one debug channel can be set at any time. Attempts to call this method
  # with a value other than nil while a debug channel is already set will raise
  # an ArgumentError.

  def self.debug_channel=(channel)
    if channel and @debug_channel
      raise ArgumentError, "A VM debug channel has already been registered"
    end
    @debug_channel = channel
  end

  def self.debug_channel
    @debug_channel
  end
end
