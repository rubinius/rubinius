module Process
  Rubinius::Globals.read_only :$?

  def self.set_status_global(status)
    Rubinius::Globals.set! :$?, status
  end

  def self.exec(cmd, *args)
    if args.empty? and cmd.kind_of? String
      raise Errno::ENOENT if cmd.empty?
      Process.perform_exec cmd, []
    else
      if ary = Rubinius::Type.try_convert(cmd, Array, :to_ary)
        raise ArgumentError, "wrong first argument" unless ary.size == 2
        prog = ary[0]
        name = ary[1]
      else
        name = prog = cmd
      end

      argv = [name]
      args.each do |arg|
        argv << arg.to_s
      end

      Process.perform_exec prog, argv
    end
  end

  # TODO: Should an error be raised on ECHILD? --rue
  #
  # TODO: This operates on the assumption that waiting on
  #       the event consumes very little resources. If this
  #       is not the case, the check should be made WNOHANG
  #       and called periodically.
  #
  def self.detach(pid)
    raise ArgumentError, "Only positive pids may be detached" unless pid > 0

    # The evented system does not need a loop
    Thread.new { Process.wait pid; $? }
  end

  def self.coerce_rlimit_resource(resource)
    Rubinius::Type.coerce_to resource, Integer, :to_int
  end
  private_class_method :coerce_rlimit_resource
end

module Kernel
  def system(prog, *args)
    pid = Process.fork
    if pid
      Process.waitpid(pid)
      $?.exitstatus == 0
    else
      begin
        Kernel.exec(prog, *args)
      rescue Exception => e
        if $DEBUG
          e.render("Unable to execute subprogram", STDERR)
        end
        exit! 1
      end

      if $DEBUG
        STDERR.puts "Unable to execute subprogram - exec silently returned"
      end
      exit! 1
    end
  end
  module_function :system
end
