require File.expand_path('../../spec_helper', __FILE__)

describe "A While node" do
  pre_while = lambda do |g|
    top    = g.new_label
    rdo    = g.new_label
    brk    = g.new_label
    post   = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!
    g.push :self
    g.send :a, 0, true
    g.gif bottom

    rdo.set!
    g.push :self
    g.send :b, 0, true
    g.push 1
    g.send :+, 1, false

    post.set!
    g.pop
    g.check_interrupts
    g.goto top

    bottom.set!
    g.push :nil

    brk.set!
    g.pop_modifiers
  end

  relates <<-ruby do
      while a
        b + 1
      end
    ruby

    compile(&pre_while)
  end

  ruby_version_is "1.9" do
    while_not = lambda do |g|
      top    = g.new_label
      rdo    = g.new_label
      brk    = g.new_label
      post   = g.new_label
      bottom = g.new_label

      g.push_modifiers

      top.set!
      g.push :self
      g.send :a, 0, true
      g.send :"!", 0, false
      g.gif bottom

      rdo.set!
      g.push :self
      g.send :b, 0, true
      g.push 1
      g.send :+, 1, false

      post.set!
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

      compile(&while_not)
    end

    relates "b + 1 while not a" do
      compile(&while_not)
    end

    relates <<-ruby do
        begin
          b + 1
        end while not a
      ruby

      compile do |g|
        top    = g.new_label
        brk    = g.new_label
        post   = g.new_label
        bottom = g.new_label

        g.push_modifiers

        top.set!

        g.push :self
        g.send :b, 0, true
        g.push 1
        g.send :+, 1, false

        post.set!
        g.pop
        g.check_interrupts

        g.push :self
        g.send :a, 0, true
        g.send :"!", 0, false
        g.gif bottom

        g.goto top

        bottom.set!
        g.push :nil

        brk.set!
        g.pop_modifiers
      end
    end
  end

  relates <<-ruby do
      while 1
        next 2
      end
    ruby

    compile do |g|
      top    = g.new_label
      rdo    = g.new_label
      brk    = g.new_label
      post   = g.new_label
      bottom = g.new_label

      g.push_modifiers

      top.set!
      g.push 1
      g.gif bottom

      rdo.set!
      g.push 2
      g.goto post

      post.set!
      g.pop
      g.check_interrupts
      g.goto top

      bottom.set!
      g.push :nil

      brk.set!
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      a = x
      while a.b
        1
      end
    ruby

    compile do |g|
      top    = g.new_label
      rdo    = g.new_label
      brk    = g.new_label
      post   = g.new_label
      bottom = g.new_label

      g.push :self
      g.send :x, 0, true
      g.set_local 0
      g.pop

      g.push_modifiers
      top.set!

      g.push_local 0
      g.send :b, 0, false
      g.gif bottom

      rdo.set!
      g.push 1

      post.set!
      g.pop
      g.check_interrupts
      g.goto top

      bottom.set!
      g.push :nil

      brk.set!
      g.pop_modifiers
    end
  end

  relates "b + 1 while a" do
    compile(&pre_while)
  end

  ruby_version_is ""..."1.9" do
    relates <<-ruby do
        until not a
          b + 1
        end
      ruby

      compile(&pre_while)
    end

    relates "b + 1 until not a" do
      compile(&pre_while)
    end
  end

  post_while = lambda do |g|
    top    = g.new_label
    brk    = g.new_label
    post   = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!

    g.push :self
    g.send :b, 0, true
    g.push 1
    g.send :+, 1, false

    post.set!
    g.pop
    g.check_interrupts

    g.push :self
    g.send :a, 0, true
    g.gif bottom

    g.goto top

    bottom.set!
    g.push :nil

    brk.set!
    g.pop_modifiers
  end

  relates <<-ruby do
      begin
        b + 1
      end while a
    ruby

    compile(&post_while)
  end

  ruby_version_is ""..."1.9" do
    relates <<-ruby do
        begin
          b + 1
        end until not a
      ruby

      compile(&post_while)
    end
  end

  nil_condition = lambda do |g|
    top    = g.new_label
    rdo    = g.new_label
    brk    = g.new_label
    post   = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!
    g.push :nil
    g.gif bottom

    rdo.set!
    g.push :self
    g.send :a, 0, true

    post.set!
    g.pop
    g.check_interrupts
    g.goto top

    bottom.set!
    g.push :nil

    brk.set!
    g.pop_modifiers
  end

  relates "a while ()" do
    compile(&nil_condition)
  end

  relates <<-ruby do
      while ()
        a
      end
    ruby

    compile(&nil_condition)
  end

  ruby_version_is ""..."1.9" do
    relates "a until not ()" do
      compile(&nil_condition)
    end

    relates <<-ruby do
        until not ()
          a
        end
      ruby

      compile(&nil_condition)
    end

    relates "a until ! ()" do
      compile(&nil_condition)
    end

    relates <<-ruby do
        until ! ()
          a
        end
      ruby

      compile(&nil_condition)
    end

  end

  relates <<-ruby do
      while 1
        2
        break :brk
      end
    ruby

    compile do |g|
      top    = g.new_label
      rdo    = g.new_label
      brk    = g.new_label
      post   = g.new_label
      bottom = g.new_label

      g.push_modifiers
      top.set!

      g.push 1
      g.gif bottom

      rdo.set!
      g.push 2
      g.pop
      g.push_literal :brk
      g.goto brk

      post.set!
      g.pop
      g.check_interrupts
      g.goto top

      bottom.set!
      g.push :nil

      brk.set!
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      while 1
        begin
          2
          break :brk
        rescue
          3
        end
      end
    ruby

    compile do |g|
      top = g.new_label
      post = g.new_label
      bottom = g.new_label
      brk = g.break = g.new_label

      g.push_modifiers
      top.set!

      g.push 1
      g.gif bottom

      # redo
      g.new_label.set!

      g.for_rescue do |rb|
        rb.body do
          g.push 2
          g.pop
          g.push_literal :brk
          rb.break
        end

        rb.condition :StandardError do
          g.push 3
        end
      end

      post.set!
      g.pop
      g.check_interrupts
      g.goto top

      bottom.set!
      g.push :nil

      brk.set!
      g.pop_modifiers
    end

  end

  relates <<-ruby do
      while 1
        begin
          2
        rescue
          3
          break :brk
        end
      end
    ruby

    compile do |g|
      top = g.new_label
      post = g.new_label
      bottom = g.new_label
      brk = g.break = g.new_label

      g.push_modifiers
      top.set!

      g.push 1
      g.gif bottom

      # redo
      g.new_label.set!

      g.for_rescue do |rb|
        rb.body do
          g.push 2
        end

        rb.condition :StandardError do
          g.push 3
          g.pop
          g.push_literal :brk
          rb.break
        end
      end

      post.set!
      g.pop
      g.check_interrupts
      g.goto top

      bottom.set!
      g.push :nil

      brk.set!
      g.pop_modifiers
    end
  end
end
