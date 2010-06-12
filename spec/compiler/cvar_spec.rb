require File.expand_path('../../spec_helper', __FILE__)

describe "A Cvar node" do
  relates "@@x" do
    compile do |g|
      g.push_scope
      g.push_literal :@@x
      g.send :class_variable_get, 1
    end
  end

  relates <<-ruby do
      class A
        @@a
      end
    ruby

    compile do |g|
      in_class :A do |d|
        d.push :self
        d.push_literal :@@a
        d.send :class_variable_get, 1
      end
    end
  end

  relates <<-ruby do
      module M
        @@a
      end
    ruby

    compile do |g|
      in_module :M do |d|
        d.push :self
        d.push_literal :@@a
        d.send :class_variable_get, 1
      end
    end
  end
end
