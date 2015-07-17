class ThreadGroup
  def initialize
    @threads = []
    @enclosed = false
  end

  Default = ThreadGroup.new

  def add(thread)
    if g = thread.group
      raise ThreadError, "can't move from the enclosed thread group" if g.enclosed?

      gm = Rubinius::Mirror.reflect g
      gm.remove thread
    end

    tm = Rubinius::Mirror.reflect thread
    tm.group = self

    Rubinius.synchronize(@threads) { @threads << thread }

    self
  end

  def list
    @threads
  end

  def enclose
    @enclosed = true
    self
  end

  def enclosed?
    @enclosed
  end
end
