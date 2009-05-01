require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using kernel_methods plugin" do
  relates "4 / 2" do
    parse do
      [:call, [:lit, 4], :/, [:arglist, [:lit, 2]]]
    end

    compile :kernel_methods do |g|
      g.push 4
      g.push 2
      g.send :divide, 1, false
    end
  end

  relates "a.class" do
    parse do
      [:call, [:call, nil, :a, [:arglist]], :class, [:arglist]]
    end

    compile :kernel_methods do |g|
      g.push :self
      g.send :a, 0, true
      g.send :__class__, 0, false
    end
  end
end

describe "A Call node NOT using kernel_methods plugin" do
  relates "4 / 2" do
    parse do
      [:call, [:lit, 4], :/, [:arglist, [:lit, 2]]]
    end

    compile do |g|
      g.push 4
      g.push 2
      g.send :/, 1, false
    end
  end

  relates "a.class" do
    parse do
      [:call, [:call, nil, :a, [:arglist]], :class, [:arglist]]
    end

    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.send :class, 0, false
    end
  end
end
