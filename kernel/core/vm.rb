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
end
