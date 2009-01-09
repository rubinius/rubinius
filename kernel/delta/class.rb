class Class
  def opened_class
    cls = superclass || Object
    cls.add_subclass self

    cls.send :inherited, self

    self # FIXME: hook calls should preserve the stack
  end

  ##
  #--
  # NOTE: This method is not standard Ruby; JRuby implements it, but not
  # public
  #++

  def add_subclass(cls)
    @subclasses ||= []
    @subclasses << cls
  end

  ##
  #--
  # NOTE: This method is not standard Ruby; JRuby implements it, but not
  # public
  #++

  def __subclasses__(descend = false)
    if descend
      subclasses_descend()
    elsif @subclasses
      @subclasses.dup
    else
      []
    end
  end

  def subclasses_descend(all = [])
    return unless @subclasses
    @subclasses.each {|cls| all << cls; cls.subclasses_descend(all)}
    all
  end

  alias_method :dup, :clone
end

cm = Class.method_table[:new]
cm.serial = CompiledMethod::KernelMethodSerial
