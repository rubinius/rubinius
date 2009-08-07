require File.dirname(__FILE__) + '/../spec_helper'

describe "A Module node" do
  relates <<-ruby do
      module X
        def y
        end
      end
    ruby

    parse do
      [:module, :X, [:scope, [:defn, :y, [:args], [:scope, [:block, [:nil]]]]]]
    end

    compile do |g|
      in_module :X do |d|
        d.in_method :y do |d2|
          d2.push :nil
        end
      end
    end
  end

  relates <<-ruby do
      module ::Y
        c
      end
    ruby

    parse do
      [:module, [:colon3, :Y], [:scope, [:call, nil, :c, [:arglist]]]]
    end

    compile do |g|
      g.push_const :Rubinius
      g.push_literal :Y
      g.push_cpath_top
      g.send :open_module_under, 2
      g.dup
      g.push_const :Rubinius
      g.swap
      g.push_literal :__module_init__
      g.swap
      g.push_literal_desc :Y do |d|
        d.push_self
        d.add_scope
        d.push :self
        d.send :c, 0, true
        d.ret
      end

      g.swap
      g.push_scope
      g.swap
      g.send :attach_method, 4
      g.pop
      g.send :__module_init__, 0
    end
  end

  relates <<-ruby do
      module X::Y
        c
      end
    ruby

    parse do
      [:module, [:colon2, [:const, :X], :Y], [:scope, [:call, nil, :c, [:arglist]]]]
    end

    compile do |g|
      in_module "X::Y" do |d|
        d.push :self
        d.send :c, 0, true
      end
    end
  end

  relates <<-ruby do
      "prevent the above from infecting rdoc"

      module Graffle
      end
    ruby

    parse do
      [:module, :Graffle, [:scope]]
    end

    compile do |g|
      g.in_module :Graffle
      g.pop
      g.push :nil
    end
  end
end
