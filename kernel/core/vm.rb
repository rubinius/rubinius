class Rubinius::VM
  def self.spawn_prim(args)
    Ruby.primitive :machine_new
  end

  def self.spawn(*args)
    ret = spawn_prim(args)
    return new(*ret)
  end

  def self.join(id)
    Ruby.primitive :machine_join
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
end
