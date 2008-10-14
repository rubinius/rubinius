module Kernel
  def method_missing(meth, *args)
    # Exclude method_missing from the backtrace since it only confuses
    # people.
    myself = MethodContext.current
    ctx = myself.sender

    if myself.send_private?
      Kernel.raise NameError, "undefined local variable or method `#{meth}' for #{inspect}"
    elsif self.__kind_of__ Class or self.__kind_of__ Module
      Kernel.raise NoMethodError.new("No method '#{meth}' on #{self} (#{self.__class__})", ctx, args)
    else
      Kernel.raise NoMethodError.new("No method '#{meth}' on an instance of #{self.__class__}.", ctx, args)
    end
  end

  private :method_missing

  # Add in $! in as a hook, to just do $!. This is for accesses to $!
  # that the compiler can't see.
  get = proc { $! }
  Globals.set_hook(:$!, get, nil)

  # Same as $!, for any accesses we might miss.
  # HACK. I doubt this is correct, because of how it will be called.
  get = proc { Regex.last_match }
  Globals.set_hook(:$~, get, nil)

  get = proc { ARGV }
  Globals.set_hook(:$*, get, nil)

  get = proc { $! ? $!.backtrace : nil }
  Globals.set_hook(:$@, get, nil)

  get = proc { Process.pid }
  Globals.set_hook(:$$, get, nil)
end
