require File.expand_path('../../spec_helper', __FILE__)

describe "An If node" do
  relates <<-ruby do
      if true then
        10
      else
        12
      end
    ruby

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
    compile do |g|
      else1 = g.new_label
      else2 = g.new_label
      done1 = g.new_label
      done2 = g.new_label

      g.push :true
      g.gif else1
      g.push :nil
      g.goto done1

      else1.set!
      g.push :false
      g.gif else2
      g.push :nil
      g.ret
      g.goto done2

      else2.set!
      g.push :nil

      done2.set!
      done1.set!
    end
  end

  relates "a if not b" do
    compile do |g|
      else_label = g.new_label
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.gif else_label
      g.push :nil
      g.goto done

      else_label.set!
      g.push :self
      g.send :a, 0, true

      done.set!
    end
  end

  relates "a if b" do
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
    compile do |g|
      else_label = g.new_label
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.gif else_label
      g.push :nil
      g.goto done

      else_label.set!
      g.push :self
      g.send :a, 0, true

      done.set!
    end
  end

  relates "if b then a end" do
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

  nil_condition_sexp = [:if, [:nil], [:call, nil, :a, [:arglist]], nil]

  nil_condition = lambda do |g|
    f = g.new_label
    done = g.new_label

    g.push :nil
    g.gif f

    g.push :self
    g.send :a, 0, true
    g.goto done

    f.set!
    g.push :nil

    done.set!
  end

  relates "a if ()" do
    compile(&nil_condition)
  end

  relates "if () then a end" do
    compile(&nil_condition)
  end

  relates "a unless not ()" do
    compile(&nil_condition)
  end

  relates "unless not () then a end" do
    compile(&nil_condition)
  end

  relates "a unless not b" do
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
    compile do |g|
      else_label = g.new_label
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.gif else_label
      g.push :nil
      g.goto done

      else_label.set!
      g.push :self
      g.send :a, 0, true

      done.set!
    end
  end

  relates "unless not b then a end" do
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
    compile do |g|
      else_label = g.new_label
      done = g.new_label

      g.push :self
      g.send :b, 0, true
      g.gif else_label
      g.push :nil
      g.goto done

      else_label.set!
      g.push :self
      g.send :a, 0, true

      done.set!
    end
  end
end
