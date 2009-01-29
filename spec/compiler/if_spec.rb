require File.dirname(__FILE__) + '/../spec_helper'

describe "An If node" do
  relates <<-ruby do
      if true then
        10
      else
        12
      end
    ruby

    parse do
      [:if, [:true], [:lit, 10], [:lit, 12]]
    end

    compile do |g|
      g.push :true
      els = g.new_label
      fin = g.new_label
      g.gif els
      g.push 10
      g.goto fin
      els.set!
      g.push 12
      fin.set!
    end
  end

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

    compile do |g|
      f      = g.new_label
      bottom = g.new_label

      g.push 5
      g.set_local 0
      g.pop

      g.push_local 0
      g.push 1
      g.send :+, 1, false
      g.gif f

      g.push :nil
      g.goto bottom

      f.set!
      g.push :nil

      bottom.set!
    end
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

    compile do |g|
      f      = g.new_label
      bottom = g.new_label

      g.push :self
      g.send :obj, 0, true
      g.send :x, 0, false
      g.set_local 0

      g.gif f

      g.push_local 0
      g.send :do_it, 0, false
      g.goto bottom

      f.set!
      g.push :nil

      bottom.set!
    end
  end

  relates "return if false unless true" do
    parse do
      [:if, [:true], nil, [:if, [:false], [:return], nil]]
    end

    compile do |g|
      yep  = g.new_label
      done = g.new_label
      inner_done = g.new_label
      nope = g.new_label

      g.push :true
      g.git  yep

      g.push :false
      g.gif  nope

      g.push :nil
      g.ret
      g.goto inner_done

      nope.set!
      g.push :nil

      inner_done.set!

      g.goto done

      yep.set!
      g.push :nil

      done.set!
    end
  end

  relates "a if not b" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], nil, [:call, nil, :a, [:arglist]]]
    end

    compile do |g|
      yep  = g.new_label
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.git yep

      g.push :self
      g.send :a, 0, true
      g.goto done

      yep.set!
      g.push :nil

      done.set!
    end
  end

  relates "a if b" do
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

  relates "if not b then a end" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], nil, [:call, nil, :a, [:arglist]]]
    end

    compile do |g|
      yep  = g.new_label
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.git yep

      g.push :self
      g.send :a, 0, true
      g.goto done

      yep.set!
      g.push :nil

      done.set!
    end
  end

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

  relates "a unless not b" do
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

  relates "a unless b" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], nil, [:call, nil, :a, [:arglist]]]
    end

    compile do |g|
      yep  = g.new_label
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.git  yep

      g.push :self
      g.send :a, 0, true
      g.goto done

      yep.set!
      g.push :nil

      done.set!
    end
  end

  relates "unless not b then a end" do
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

  relates "unless b then a end" do
    parse do
      [:if, [:call, nil, :b, [:arglist]], nil, [:call, nil, :a, [:arglist]]]
    end

    compile do |g|
      yep  = g.new_label
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.git  yep

      g.push :self
      g.send :a, 0, true
      g.goto done

      yep.set!
      g.push :nil

      done.set!
    end
  end
end
