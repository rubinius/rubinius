require File.expand_path('../../spec_helper', __FILE__)

describe "An Until node" do
  pre_until = lambda do |g|
    top    = g.new_label
    rdo    = g.new_label
    brk    = g.new_label
    post   = g.new_label
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

    post.set!
    g.pop
    g.check_interrupts
    g.goto top

    bottom.set!
    g.push :nil

    brk.set!
    g.pop_modifiers
  end

  post_until = lambda do |g|
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
    g.git bottom

    g.goto top

    bottom.set!
    g.push :nil

    brk.set!

    g.pop_modifiers
  end

  ruby_version_is ""..."1.9" do
    nil_condition = lambda do |g|
      top    = g.new_label
      rdo    = g.new_label
      brk    = g.new_label
      post   = g.new_label
      bottom = g.new_label

      g.push_modifiers
      top.set!

      g.push :nil
      g.git bottom

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
        begin
          b + 1
        end while not a
      ruby

      compile(&post_until)
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

    relates "a while ! ()" do
      compile(&nil_condition)
    end

    relates <<-ruby do
        while ! ()
          a
        end
      ruby

      compile(&nil_condition)
    end
  end

  ruby_version_is "1.9" do
    nil_condition = lambda do |g|
      top    = g.new_label
      rdo    = g.new_label
      brk    = g.new_label
      post   = g.new_label
      bottom = g.new_label

      g.push_modifiers
      top.set!

      g.push :nil
      g.git bottom

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

    not_nil_condition = lambda do |g|
      top    = g.new_label
      rdo    = g.new_label
      brk    = g.new_label
      post   = g.new_label
      bottom = g.new_label

      g.push_modifiers
      top.set!

      g.push :nil
      g.send :"!", 0, false
      g.git bottom

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

    relates "a until ! ()" do
      compile(&not_nil_condition)
    end

    relates <<-ruby do
        until ! ()
          a
        end
      ruby

      compile(&not_nil_condition)
    end
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

  relates <<-ruby do
      begin
        b + 1
      end until a
    ruby

    compile(&post_until)
  end
end
