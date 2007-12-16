class Object
  def shared(msg, &block)
    Object.instance_variable_set(:"@#{msg}", Proc.new(&block))
  end

  def it_behaves_like(behavior, meth)
    Object.instance_variable_get(:"@#{behavior}").call(meth)
  end
end
