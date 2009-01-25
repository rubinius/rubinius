require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using safemath plugin" do
  relates "4 / 2" do
    parse do
      [:call, [:lit, 4], :/, [:arglist, [:lit, 2]]]
    end

    compile :safemath do |g|
      g.push 4
      g.push 2
      g.send :divide, 1, false
    end
  end
end

describe "A Call node NOT using safemath plugin" do
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
end
