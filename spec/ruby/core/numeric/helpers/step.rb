class Object
  def get_args(args_type, *args)
    if(args_type == :kw)
      kw_args = {to: args[0]}
      kw_args[:by] = args[1] if args.size == 2
      [kw_args]
    elsif args_type == :mix && args.size == 2
      [args[0], {by: args[1]}]
    else
      args
    end
  end
end