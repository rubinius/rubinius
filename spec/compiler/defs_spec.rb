require File.dirname(__FILE__) + '/../spec_helper'

describe "A Defs node" do
  relates <<-ruby do
      def self.x(y)
        (y + 1)
      end
    ruby

    parse do
      [:defs,
         [:self],
         :x,
         [:args, :y],
         [:scope, [:block, [:call, [:lvar, :y], :+, [:arglist, [:lit, 1]]]]]]
    end

    compile do |g|
      g.push :self
      in_method :x, true do |d|
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

    parse do
      [:defs,
         [:self],
         :setup,
         [:args, :ctx],
         [:scope,
          [:block,
           [:lasgn, :bind, [:call, nil, :allocate, [:arglist]]],
           [:attrasgn, [:lvar, :bind], :context=, [:arglist, [:lvar, :ctx]]],
           [:return, [:lvar, :bind]]]]]
    end

    compile do |g|
      g.push :self
      g.in_method :setup, true do |d|
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

    parse do
      [:defs, [:self], :empty, [:args, :*], [:scope, [:block]]]
    end

    compile do |g|
      g.push :self
      in_method :empty, true do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def self.empty
      end
    ruby

    parse do
      [:defs, [:self], :empty, [:args], [:scope, [:block]]]
    end

    compile do |g|
      g.push :self
      in_method :empty, true do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def (a.b).empty(*)
      end
    ruby

    parse do
      [:defs,
       [:call, [:call, nil, :a, [:arglist]], :b, [:arglist]],
       :empty,
       [:args, :*],
       [:scope, [:block]]]
    end

    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.send :b, 0, false

      in_method :empty, true do |d|
        d.push :nil
      end
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
      g.push_local 0

      in_method :m, true do |d|
        d.push_local 0
      end
    end
  end
end
