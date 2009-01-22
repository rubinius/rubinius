require File.dirname(__FILE__) + '/../spec_helper'

describe "A Class node" do
  relates <<-ruby do
      class X
        puts((1 + 1))
        def blah
          puts("hello")
        end
      end
    ruby

    parse do
      [:class,
       :X,
       nil,
       [:scope,
        [:block,
         [:call,
          nil,
          :puts,
          [:arglist, [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]]],
         [:defn,
          :blah,
          [:args],
          [:scope,
           [:block, [:call, nil, :puts, [:arglist, [:str, "hello"]]]]]]]]]
    end

    # class plain
  end

  relates <<-ruby do
      class ::Y
        c
      end
    ruby

    parse do
      [:class, [:colon3, :Y], nil, [:scope, [:call, nil, :c, [:arglist]]]]
    end

    # class scoped3
  end

  relates <<-ruby do
      class X::Y
        c
      end
    ruby

    parse do
      [:class,
       [:colon2, [:const, :X], :Y],
       nil,
       [:scope, [:call, nil, :c, [:arglist]]]]
    end

    # class scoped
  end

  relates <<-ruby do
      class X < Array
      end
    ruby

    parse do
      [:class, :X, [:const, :Array], [:scope]]
    end

    # class super array
  end

  relates <<-ruby do
      class X < expr
      end
    ruby

    parse do
      [:class, :X, [:call, nil, :expr, [:arglist]], [:scope]]
    end

    # class super expr
  end

  relates <<-ruby do
      class X < Object
      end
    ruby

    parse do
      [:class, :X, [:const, :Object], [:scope]]
    end

    # class super object
  end
end
