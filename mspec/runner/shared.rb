class Object
  def shared(msg, &block)
    Object.instance_variable_set(:"@#{msg}", Proc.new(&block))
  end

  def it_behaves_like(behavior, meth, klass=nil)
    p = Object.instance_variable_get(:"@#{behavior}")
    klass ? p[meth, klass] : p[meth]
  end
end
