require File.expand_path('../../spec_helper', __FILE__)

describe "A Module node" do
  relates <<-ruby do
      module X
        def y
        end
      end
    ruby

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

    compile do |g|
      g.push_rubinius
      g.push_literal :Y
      g.push_cpath_top
      g.send :open_module_under, 2

      d = new_generator(g, :Y)
      g.create_block d

      d.push_self
      d.add_scope
      d.push :self
      d.send :c, 0, true
      d.ret

      g.swap
      g.push_scope
      g.push_true
      g.send :call_under, 3
    end
  end

  relates <<-ruby do
      module X::Y
        c
      end
    ruby

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

    compile do |g|
      g.in_module :Graffle
      g.pop
      g.push :nil
    end
  end
end
