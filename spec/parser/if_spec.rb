require File.dirname(__FILE__) + '/../spec_helper'

describe "An If node" do
  relates "if b then a end" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], [:call, nil, :a, [:arglist]], nil]
    end

    compile do |g|
      nope = g.new_label
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.gif nope

      g.push :self
      g.send :a, 0, true
      g.goto done

      nope.set!
      g.push :nil

      done.set!
    end
  end

  relates <<-ruby do
      if (x = 5
      (x + 1)) then
        nil
      end
    ruby

    parse do
      [:if,
       [:block,
        [:lasgn, :x, [:lit, 5]],
        [:call, [:lvar, :x], :+, [:arglist, [:lit, 1]]]],
       [:nil],
       nil]
    end

    # if block condition
  end

  relates <<-ruby do
      if x = obj.x then
        x.do_it
      end
    ruby

    parse do
      [:if,
       [:lasgn, :x, [:call, [:call, nil, :obj, [:arglist]], :x, [:arglist]]],
       [:call, [:lvar, :x], :do_it, [:arglist]],
       nil]
    end

    # if lasgn short
  end

  relates "return if false unless true" do
    parse do
      [:if, [:true], nil, [:if, [:false], [:return], nil]]
    end

    # if nested
  end

  relates "a if not b" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], nil, [:call, nil, :a, [:arglist]]]
    end

    # if post not
  end

  relates "a if b" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], [:call, nil, :a, [:arglist]], nil]
    end

    # if post
  end

  relates "if not b then a end" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], nil, [:call, nil, :a, [:arglist]]]
    end

    # if pre not
  end

  relates "if b then a end" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], [:call, nil, :a, [:arglist]], nil]
    end

    # if pre
  end

  relates "a unless not b" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], [:call, nil, :a, [:arglist]], nil]
    end

    # unless post not
  end

  relates "a unless b" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], nil, [:call, nil, :a, [:arglist]]]
    end

    # unless post
  end

  relates "unless not b then a end" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], [:call, nil, :a, [:arglist]], nil]
    end

    # unless pre not
  end

  relates "unless b then a end" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], nil, [:call, nil, :a, [:arglist]]]
    end

    # unless pre
  end
end
