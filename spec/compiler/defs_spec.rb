require File.dirname(__FILE__) + '/../spec_helper'

describe "A Defs node" do
  relates <<-ruby do
      def self.x(y)
        (y + 1)
      end
    ruby

    compile do |g|
      in_singleton_method :x, [:push, :self] do |d|
        d.push_local 0
        d.push 1
        d.send :+, 1, false
      end
    end
  end

  relates <<-ruby do
      def self.setup(ctx)
        bind = allocate
        bind.context = ctx
        return bind
      end
    ruby

    compile do |g|
      g.in_singleton_method :setup, [:push, :self] do |d|
        d.push :self
        d.send :allocate, 0, true
        d.set_local 1
        d.pop

        d.push_local 1
        d.push_local 0
        d.dup
        d.move_down 2
        d.send :context=, 1, false
        d.pop
        d.pop

        d.push_local 1
        d.ret # TODO: why extra return?
      end
    end
  end

  relates <<-ruby do
      def self.empty(*)
      end
    ruby

    compile do |g|
      in_singleton_method :empty, [:push, :self] do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def self.empty
      end
    ruby

    compile do |g|
      in_singleton_method :empty, [:push, :self] do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def (a.b).empty(*)
      end
    ruby

    compile do |g|
      g.push_rubinius

      g.push_literal :empty

      d = new_generator(g, name)
      d.push :nil
      d.ret

      g.push_literal(d)
      g.push_scope

      g.push :self
      g.send :a, 0, true
      g.send :b, 0, false

      g.send :attach_method, 4
    end
  end

  relates <<-ruby do
    x = "a"
    def x.m(a)
      a
    end
    ruby

    compile do |g|
      g.push_literal "a"
      g.string_dup
      g.set_local 0
      g.pop

      in_singleton_method :m, [:push_local, 0] do |d|
        d.push_local 0
      end
    end
  end
end
