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
