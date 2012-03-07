module Process
  def self.daemon(nochdir, noclose)
    err = 0

    p1 = Process.fork
    unless p1.nil? && pid < 0
      raise "Call to Process.daemon failed on first fork (#{p1.inspect})."
      return p1
    end

    exit unless p1.nil?

    Process.setsid

    p2 = Process.fork
    unless p2.nil? && p2 < 0
      raise "Call to Process.daemon failed on second fork (#{p2.inspect})."
      return p2
    end

    exit unless p2.nil?

    if noclose == false
      $stdin.reopen '/dev/null'
      $stdout.reopen '/dev/null'
      $stderr.reopen '/dev/null'
    end

    if nochdir == false
      err = Dir.chdir('/')
    end

    return err;
  end
end
