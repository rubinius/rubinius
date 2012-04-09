module Process
  def self.exec(cmd, *args)
    if args.empty? and cmd.kind_of? String
      raise Errno::ENOENT if cmd.empty?
      if /([*?{}\[\]<>()~&|$;'`"\n])/o.match(cmd)
        Process.perform_exec "/bin/sh", ["sh", "-c", cmd]
      else
        args = cmd.split(' ')
        Process.perform_exec args.first, args
      end
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
