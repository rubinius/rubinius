require File.expand_path('../../spec_helper', __FILE__)

describe "A Cvdecl node" do
  relates <<-ruby do
      class X
        @@blah = 1
      end
    ruby

    compile do |g|
      in_class :X do |d|
        d.push :self
        d.push_literal :@@blah
        d.push 1
        d.send :class_variable_set, 2
      end
    end
  end
end
