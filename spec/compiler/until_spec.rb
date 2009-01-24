require File.dirname(__FILE__) + '/../spec_helper'

describe "An Until node" do
  relates <<-ruby do
      begin
        (1 + 1)
      end until false
    ruby

    parse do
      [:until, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], false]
    end

    compile do |g|
      top    = g.new_label
      nxt    = g.new_label
      brek   = g.new_label
      bottom = g.new_label

      g.push_modifiers

      top.set!

      g.push 1
      g.push 1
      g.meta_send_op_plus
      g.pop

      nxt.set!

      g.push :false
      g.git bottom

      g.goto top

      bottom.set!
      g.push :nil

      brek.set!

      g.pop_modifiers
    end
  end

  until_false = lambda do |g|
    top    = g.new_label
    dunno1 = g.new_label
    dunno2 = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!
    g.push :false
    g.git bottom

    dunno1.set!
    g.push 1
    g.push 1
    g.meta_send_op_plus
    g.pop

    g.goto top

    bottom.set!
    g.push :nil

    dunno2.set!

    g.pop_modifiers
  end

  relates "(1 + 1) until false" do
    parse do
      [:until, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    compile(&until_false)
  end

  relates <<-ruby do
      until false do
        (1 + 1)
      end
    ruby

    parse do
      [:until, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    compile(&until_false)
  end

  until_true = lambda do |g|
    top    = g.new_label
    dunno1 = g.new_label
    dunno2 = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!
    g.push :true
    g.git bottom

    dunno1.set!
    g.push 1
    g.push 1
    g.meta_send_op_plus
    g.pop

    g.goto top

    bottom.set!
    g.push :nil

    dunno2.set!

    g.pop_modifiers
  end

  relates <<-ruby do
      while not true do
        (1 + 1)
      end
    ruby

    parse do
      [:until, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    compile(&until_true)
  end

  relates "(1 + 1) while not true" do
    parse do
      [:until, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    compile(&until_true)
  end

  relates <<-ruby do
      begin
        (1 + 1)
      end while not true
    ruby

    parse do
      [:until, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], false]
    end

    compile do |g|
      top    = g.new_label
      dunno1 = g.new_label
      dunno2 = g.new_label
      bottom = g.new_label

      g.push_modifiers

      top.set!
      g.push 1
      g.push 1
      g.meta_send_op_plus
      g.pop

      dunno1.set!
      g.push :true
      g.git bottom

      g.goto top

      bottom.set!
      g.push :nil

      dunno2.set!

      g.pop_modifiers
    end
  end
end
