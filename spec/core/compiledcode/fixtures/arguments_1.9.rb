module CompiledCodeSpecs
  MethodOM  = instance_method(def method_om(a=1,b); end).executable
  MethodMOM = instance_method(def method_mom(a, b=1, c); end).executable
end
