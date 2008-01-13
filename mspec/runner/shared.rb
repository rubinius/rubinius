class Object
  def shared(msg, &block)
    Object.instance_variable_set(:"@#{msg}", Proc.new(&block))
  end

  def it_behaves_like(behavior, *args)
    p = Object.instance_variable_get(:"@#{behavior}")
    p[*args]
  end
end
