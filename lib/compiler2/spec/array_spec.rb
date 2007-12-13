require File.dirname(__FILE__) + "/helper.rb"

describe Compiler do
  it "compiles '[1, 2, *foo()]" do
    x = [:argscat,
      [:array, [:lit, 1], [:lit, 2]],
      [:fcall, :foo]
    ]

    gen x do |g|
      g.push :self
      g.send :foo, 0, true
      g.cast_array
      g.push 1
      g.push 2
      g.make_array 2
      g.send :+, 1
    end
  end
  
  it "coerces the array reference and concatenates it to the first array" do
    x = [:argscat,
            [:array, [:lit, 1], [:lit, 2]], 
            [:lvar,  :x, 0]]
    
    gen x do |g|
      g.push_local 0
      g.cast_array
      g.push 1
      g.push 2
      g.make_array 2
      g.send :+, 1
    end
  end
  
end
