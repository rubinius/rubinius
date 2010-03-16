require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using InvokePrimitive transform" do
  relates <<-ruby do
      Rubinius.invoke_primitive :name, 1, 2, a
    ruby

    compile :invoke_primitive do |g|
      g.push 1
      g.push 2
      g.push :self
      g.send :a, 0, true
      g.invoke_primitive :name, 3
    end
  end
end
