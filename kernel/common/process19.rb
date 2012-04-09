module Process
  def self.daemon(stay_in_dir=false, keep_stdio_open=false)
    # Do not run at_exit handlers in the parent
    exit!(0) if fork

    Process.setsid

    exit!(0) if fork

    Dir.chdir("/") unless stay_in_dir

    unless keep_stdio_open
      io = File.open "/dev/null", File::RDWR, 0
      $stdin.reopen io
      $stdout.reopen io
      $stderr.reopen io
    end

    return 0
  end

  def self.exec(environment_or_cmd, *args)
    if options = Rubinius::Type.try_convert(args.last, Hash, :to_hash)
      args.pop
    else
      options = {}
    end

    if env = Rubinius::Type.try_convert(environment_or_cmd, Hash, :to_hash)
      cmd = args.shift
    else
      env = {}
      cmd = environment_or_cmd
    end

    env.each { |key, value| ENV[key] = value }

    if options[:unsetenv_others]
      ENV.keep_if { |key, _| env.key?(key) }
    end

    if chdir = options[:chdir]
      Dir.chdir(chdir)
    end

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
    IO.pipe do |read, write|
      pid = Process.fork do
        read.close

        begin
          Kernel.exec(prog, *args)
        ensure
          write.write false
          exit! 1
        end
      end

      write.close
      Process.waitpid(pid)

      if read.eof?
        $?.exitstatus == 0
      else
        nil
      end
    end
  end
  module_function :system
end
