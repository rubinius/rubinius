require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using kernel_methods transform" do
  relates "4 / 2" do
    compile do |g|
      g.push 4
      g.push 2
      g.send :/, 1, false
    end

    compile :kernel_methods do |g|
      g.push 4
      g.push 2
      g.send :divide, 1, false
    end
  end

  relates "a.class" do
    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.send :class, 0, false
    end

    compile :kernel_methods do |g|
      g.push :self
      g.send :a, 0, true
      g.send :__class__, 0, false
    end
  end
end
