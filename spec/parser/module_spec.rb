require File.dirname(__FILE__) + '/../spec_helper'

describe "A Module node" do
  relates <<-ruby do
      module X
        def y
          # do nothing
        end
      end
    ruby

    parse do
      [:module, :X, [:scope, [:defn, :y, [:args], [:scope, [:block, [:nil]]]]]]
    end

    # module
  end

  relates <<-ruby do
      module ::Y
        c
      end
    ruby

    parse do
      [:module, [:colon3, :Y], [:scope, [:call, nil, :c, [:arglist]]]]
    end

    # module scoped3
  end

  relates <<-ruby do
      module X::Y
        c
      end
    ruby

    parse do
      [:module, [:colon2, [:const, :X], :Y], [:scope, [:call, nil, :c, [:arglist]]]]
    end

    # module scoped
  end

  relates <<-ruby do
      "prevent the above from infecting rdoc"

      module Graffle
      end
    ruby

    parse do
      [:module, :Graffle, [:scope]]
    end

    # structure unused literal wwtt
  end
end
