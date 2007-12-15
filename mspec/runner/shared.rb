class Object
  def shared(msg, &block)
    spec_runner.instance_variable_set(:"@#{msg}", Proc.new(&block))
  end

  def it_behaves_like(behavior, meth)
    spec_runner.instance_variable_get(:"@#{behavior}").call(meth)
  end
end
