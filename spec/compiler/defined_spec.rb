require File.dirname(__FILE__) + '/../spec_helper'

describe "A Defined node" do
  relates "defined? $x" do
    parse do
      [:defined, [:gvar, :$x]]
    end

    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push_const :Rubinius
      g.find_const :Globals
      g.push_literal :$x
      g.send :key?, 1
      g.git t

      g.push :nil
      g.goto f

      t.set!
      g.push_literal "global-variable"

      f.set!
    end
  end

  relates "defined? a" do
    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push :self
      g.push_literal :a
      g.push :true
      g.send :__respond_to_eh__, 2
      g.git t
      g.push :nil
      g.goto f

      t.set!
      g.push_literal "method"

      f.set!
    end
  end

  relates <<-ruby do
      a = 1
      defined? a
    ruby

    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push 1
      g.set_local 0
      g.pop

      g.push :true
      g.git t
      g.push :nil
      g.goto f

      t.set!
      g.push_literal "local-variable"

      f.set!
    end
  end

  relates "defined? X" do
    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push_scope
      g.push_literal :X
      g.send :const_defined?, 1
      g.git t
      g.push :nil
      g.goto f

      t.set!
      g.push_literal "constant"

      f.set!
    end
  end

  relates "defined? ::X" do
    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push_const :Object
      g.push_literal "X"
      g.send :const_path_defined?, 1
      g.git t
      g.push :nil
      g.goto f

      t.set!
      g.push_literal "constant"

      f.set!
    end
  end

  relates "defined? X::Y" do
    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push_scope
      g.push_literal "X::Y"
      g.send :const_path_defined?, 1
      g.git t
      g.push :nil
      g.goto f

      t.set!
      g.push_literal "constant"

      f.set!
    end
  end

  relates "defined? X::Y::Z" do
    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push_scope
      g.push_literal "X::Y::Z"
      g.send :const_path_defined?, 1
      g.git t
      g.push :nil
      g.goto f

      t.set!
      g.push_literal "constant"

      f.set!
    end
  end
end
