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

    # defs
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

    # block_attrasgn
  end

  relates <<-ruby do
      def self.empty(*)
        # do nothing
      end
    ruby

    parse do
      [:defs, [:self], :empty, [:args, :*], [:scope, [:block]]]
    end

    # defs empty args
  end

  relates <<-ruby do
      def self.empty
        # do nothing
      end
    ruby

    parse do
      [:defs, [:self], :empty, [:args], [:scope, [:block]]]
    end

    # defs empty
  end

  relates <<-ruby do
      def (a.b).empty(*)
        # do nothing
      end
    ruby

    parse do
      [:defs,
       [:call, [:call, nil, :a, [:arglist]], :b, [:arglist]],
       :empty,
       [:args, :*],
       [:scope, [:block]]]
    end

    # defs expr wtf
  end
end
