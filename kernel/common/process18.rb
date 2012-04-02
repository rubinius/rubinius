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
