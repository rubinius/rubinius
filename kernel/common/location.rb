class Location
  def describe
    if @method_module.equal?(Kernel)
      str = "Kernel."
    elsif @method_module.kind_of?(MetaClass)
      str = "#{@receiver}."
    elsif @method_module and @method_module != @receiver.__class__
      str = "#{@method_module}(#{@receiver.__class__})#"
    else
      str = "#{@receiver.__class__}#"
    end

    if @is_block
      str << "#{@name} {}"
    elsif @name == @method.name
      str << "#{@name}"
    else
      str << "#{@name} (#{@method.name})"
    end

    return str
  end

  # Current line being executed by the VM.
  def line
    return 0 unless @method
    # We subtract 1 because the ip is actually set to what it should do
    # next, not what it's currently doing (unless we are at the start of
    # a new context).
    ip = @ip - 1
    ip = 0 if @ip < 0
    @method.line_from_ip(ip)
  end

  ##
  # Place in the source that this method was created at.
  def position
    l = line()
    if l == 0
      "#{@method.file}+#{@ip-1}"
    else
      "#{@method.file}:#{l}"
    end
  end

end
