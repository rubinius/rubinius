require File.dirname(__FILE__) + '/../spec_helper'

describe "A Super node" do
  relates <<-ruby do
      def x
        super()
      end
    ruby

    parse do
      [:defn, :x, [:args], [:scope, [:block, [:super]]]]
    end

    # super 0
  end

  relates <<-ruby do
      def x
        super([24, 42])
      end
    ruby

    parse do
      [:defn,
       :x,
       [:args],
       [:scope, [:block, [:super, [:array, [:lit, 24], [:lit, 42]]]]]]
    end

    # super 1 array
  end

  relates <<-ruby do
      def x
        super(4)
      end
    ruby

    parse do
      [:defn, :x, [:args], [:scope, [:block, [:super, [:lit, 4]]]]]
    end

    # super 1
  end

  relates "super(a, &b)" do
    parse do
      [:super,
       [:call, nil, :a, [:arglist]],
       [:block_pass, [:call, nil, :b, [:arglist]]]]
    end

    # super block pass
  end

  relates "super(a, *b)" do
    parse do
      [:super,
        [:call, nil, :a, [:arglist]],
        [:splat, [:call, nil, :b, [:arglist]]]]
    end

    # super block splat
  end

  relates <<-ruby do
      def x
        super(24, 42)
      end
    ruby

    parse do
      [:defn,
        :x,
        [:args],
        [:scope, [:block, [:super, [:lit, 24], [:lit, 42]]]]]
    end

    # super n
  end

  relates "super([*[1]])" do
    parse do
      [:super, [:array, [:splat, [:array, [:lit, 1]]]]]
    end

    # splat super array
  end

  relates "super(*[1])" do
    parse do
      [:super, [:splat, [:array, [:lit, 1]]]]
    end

    # splat super
  end
end
