require File.dirname(__FILE__) + "/helper.rb"

describe Compiler, "processing an :argscat node with a splatted array reference" do
  
  it "coerces the array reference and concatenates it to the first array" do
    x = [:argscat,
            [:array, [:lit, 1], [:lit, 2]], 
            [:lvar,  :x]]
    
    gen x do |g|
      g.push_local 0
      g.push_const :VM
      g.send :coerce_to_array, 1, false
      g.push 2
      g.push 1
      g.make_array
      g.send :+, 1, false
    end
  end
end