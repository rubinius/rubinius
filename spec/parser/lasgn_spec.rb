require File.dirname(__FILE__) + '/../spec_helper'

describe "An Lasgn node" do
  relates "a = b, c, *d" do
    parse do
      [:lasgn,
         :a,
         [:svalue,
          [:array,
           [:call, nil, :b, [:arglist]],
           [:call, nil, :c, [:arglist]],
           [:splat, [:call, nil, :d, [:arglist]]]]]]
    end

    # argscat
  end

  relates "a = [b, *c]" do
    parse do
      [:lasgn,
         :a,
         [:array,
          [:call, nil, :b, [:arglist]],
          [:splat, [:call, nil, :c, [:arglist]]]]]
    end

    # argscat_inside
  end

  relates <<-ruby do
      x = (y = 1
           (y + 2))
    ruby

    parse do
      [:lasgn,
         :x,
         [:block,
          [:lasgn, :y, [:lit, 1]],
          [:call, [:lvar, :y], :+, [:arglist, [:lit, 2]]]]]
    end

    # block_lasgn
  end

  relates <<-ruby do
      a = []
      a [42]
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:array]],
       [:call, [:lvar, :a], :[], [:arglist, [:lit, 42]]]]
    end

    # call index space
  end

  relates 'var = ["foo", "bar"]' do
    parse do
      [:lasgn, :var, [:array, [:str, "foo"], [:str, "bar"]]]
    end

    # lasgn array
  end

  relates "c = (2 + 3)" do
    parse do
      [:lasgn, :c, [:call, [:lit, 2], :+, [:arglist, [:lit, 3]]]]
    end

    # lasgn call
  end

  relates "a = *[1]" do
    parse do
      [:lasgn, :a, [:svalue, [:splat, [:array, [:lit, 1]]]]]
    end

    # lasgn splat
  end

  relates "a = *b" do
    parse do
      [:lasgn, :a, [:svalue, [:splat, [:call, nil, :b, [:arglist]]]]]
    end

    # svalue
  end

  relates <<-ruby do
      a = if c
            begin
              b
            rescue
              nil
            end
          end
      a
    ruby

    parse do
      [:block,
       [:lasgn,
        :a,
        [:if,
         [:call, nil, :c, [:arglist]],
         [:rescue, [:call, nil, :b, [:arglist]], [:resbody, [:array], [:nil]]],
         nil]],
       [:lvar, :a]]
    end

    # structure remove begin 2
  end

  relates "x = [*[1]]" do
    parse do
      [:lasgn, :x, [:array, [:splat, [:array, [:lit, 1]]]]]
    end

    # splat lasgn array
  end

  relates "x = *[1]" do
    parse do
      [:lasgn, :x, [:svalue, [:splat, [:array, [:lit, 1]]]]]
    end

    # splat lasgn
  end

  relates "a = []" do
    parse do
      [:lasgn, :a, [:array]]
    end

    # zarray
  end
end
