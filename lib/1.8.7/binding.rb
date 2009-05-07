class Binding
  # Evaluates the Ruby expression(s) in string, in the binding‘s context.
  # If the optional filename and lineno parameters are present,
  # they will be used when reporting syntax errors. 
  def eval(expr, *arg)
    Kernel.eval(expr, self, *arg)
  end
end
