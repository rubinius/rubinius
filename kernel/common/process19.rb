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
    if environment_or_cmd.kind_of? Hash
      environment_or_cmd.each do |key, value|
        ENV[key] = value
      end

      cmd = args.shift
    else
      cmd = environment_or_cmd
    end

    if args.empty? and cmd.kind_of? String
      raise Errno::ENOENT if cmd.empty?
      if /([*?{}\[\]<>()~&|$;'`"\n\s]|[^\w-])/o.match(cmd)
        Process.perform_exec "/bin/sh", ["sh", "-c", cmd]
      else
        Process.perform_exec cmd, [cmd]
      end
    else
      if cmd.kind_of? Array
        prog = cmd[0]
        name = cmd[1]
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
