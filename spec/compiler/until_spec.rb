require File.expand_path('../../spec_helper', __FILE__)

describe "An Until node" do
  pre_until_sexp = [
    :until,
     [:call, nil, :a, [:arglist]],
     [:call, [:call, nil, :b, [:arglist]], :+, [:arglist, [:lit, 1]]],
     true
  ]

  pre_until = lambda do |g|
    top    = g.new_label
    rdo    = g.new_label
    brk    = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!
    g.push :self
    g.send :a, 0, true
    g.git bottom

    rdo.set!
    g.push :self
    g.send :b, 0, true
    g.push 1
    g.send :+, 1, false
    g.pop

    g.check_interrupts
    g.goto top

    bottom.set!
    g.push :nil

    brk.set!
    g.pop_modifiers
  end

  relates <<-ruby do
      while not a
        b + 1
      end
    ruby

    compile(&pre_until)
  end

  relates "b + 1 while not a" do
    compile(&pre_until)
  end

  relates <<-ruby do
      until a
        b + 1
      end
    ruby

    compile(&pre_until)
  end

  relates "b + 1 until a" do
    compile(&pre_until)
  end

  post_until_sexp = [
    :until,
     [:call, nil, :a, [:arglist]],
     [:call, [:call, nil, :b, [:arglist]], :+, [:arglist, [:lit, 1]]],
     false
  ]

  post_until = lambda do |g|
    top    = g.new_label
    rdo    = g.new_label
    brk    = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!

    g.push :self
    g.send :b, 0, true
    g.push 1
    g.send :+, 1, false
    g.pop

    rdo.set!

    g.push :self
    g.send :a, 0, true
    g.git bottom

    g.check_interrupts
    g.goto top

    bottom.set!
    g.push :nil

    brk.set!

    g.pop_modifiers
  end

  relates <<-ruby do
      begin
        b + 1
      end while not a
    ruby

    compile(&post_until)
  end

  relates <<-ruby do
      begin
        b + 1
      end until a
    ruby

    compile(&post_until)
  end

  nil_condition_sexp = [:until, [:nil], [:call, nil, :a, [:arglist]], true]

  nil_condition = lambda do |g|
    top    = g.new_label
    rdo    = g.new_label
    brk    = g.new_label
    bottom = g.new_label

    g.push_modifiers
    top.set!

    g.push :nil
    g.git bottom

    rdo.set!
    g.push :self
    g.send :a, 0, true
    g.pop

    g.check_interrupts
    g.goto top

    bottom.set!
    g.push :nil

    brk.set!
    g.pop_modifiers
  end

  relates "a until ()" do
    compile(&nil_condition)
  end

  relates <<-ruby do
      until ()
        a
      end
    ruby

    compile(&nil_condition)
  end

  relates "a while not ()" do
    compile(&nil_condition)
  end

  relates <<-ruby do
      while not ()
        a
      end
    ruby

    compile(&nil_condition)
  end
end
