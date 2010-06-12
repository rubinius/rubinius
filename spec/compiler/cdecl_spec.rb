require File.expand_path('../../spec_helper', __FILE__)

describe "A Cdecl node" do
  relates "X = 42" do
    compile do |g|
      g.push_scope
      g.push_literal :X
      g.push 42
      g.send :const_set, 2
    end
  end

  relates "::X = 1" do
    compile do |g|
      g.push_cpath_top
      g.push_literal :X
      g.push 1
      g.send :const_set, 2
    end
  end

  relates "X::Y = 1" do
    compile do |g|
      g.push_const :X
      g.push_literal :Y
      g.push 1
      g.send :const_set, 2
    end
  end

  relates "X::Y::Z = a" do
    compile do |g|
      g.push_const :X
      g.find_const :Y
      g.push_literal :Z
      g.push :self
      g.send :a, 0, true
      g.send :const_set, 2
    end
  end

  relates "a::A = 1" do
    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.push_literal :A
      g.push 1
      g.send :const_set, 2
    end
  end

  relates <<-ruby do
      a = Object
      a::B = b
    ruby

    compile do |g|
      g.push_const :Object
      g.set_local 0
      g.pop
      g.push_local 0
      g.push_literal :B
      g.push :self
      g.send :b, 0, true
      g.send :const_set, 2
    end
  end
end
