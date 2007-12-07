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
end
