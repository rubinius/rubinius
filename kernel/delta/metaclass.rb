class MetaClass

  ##
  # Called to perform adding a singleton method to an
  # object when the singleton method definition occurs
  # in normal user code.
  #
  def attach_method(name, executable, scope)
    # All userland added methods start out with a serial of 1.
    executable.serial = 1

    method_table.store name, executable, :public

    executable.scope = scope
    Rubinius::VM.reset_method_cache(name)

    # Call singleton_method_added on the executable in question. There is
    # a default version in Kernel which does nothing, so we can always
    # call this.
    attached_instance.__send__ :singleton_method_added, name

    executable
  end

  def inspect
    "#<MetaClass #{attached_instance.inspect}>"
  end
end
