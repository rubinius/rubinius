require File.dirname(__FILE__) + '/../spec_helper'

describe "A While node" do
  while_false = lambda do |g|
    top    = g.new_label
    dunno1 = g.new_label
    dunno2 = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!
    g.push :false
    g.gif bottom

    dunno1.set!
    g.push 1
    g.push 1
    g.send :+, 1, false
    g.pop

    g.goto top

    bottom.set!
    g.push :nil

    dunno2.set!

    g.pop_modifiers
  end

  relates <<-ruby do
      while false do
        (1 + 1)
      end
    ruby

    parse do
      [:while, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    compile(&while_false)
  end

  relates "(1 + 1) while false" do
    parse do
      [:while, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    compile(&while_false)
  end

  while_true = lambda do |g|
    top    = g.new_label
    dunno1 = g.new_label
    dunno2 = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!
    g.push :true
    g.gif bottom

    dunno1.set!
    g.push 1
    g.push 1
    g.send :+, 1, false
    g.pop

    g.goto top

    bottom.set!
    g.push :nil

    dunno2.set!

    g.pop_modifiers
  end

  relates <<-ruby do
      until not true do
        (1 + 1)
      end
    ruby

    parse do
      [:while, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    compile(&while_true)
  end

  relates "(1 + 1) until not true" do
    parse do
      [:while, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    compile(&while_true)
  end

  relates <<-ruby do
      begin
        (1 + 1)
      end until not true
    ruby

    parse do
      [:while, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], false]
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
      g.send :+, 1, false
      g.pop

      dunno1.set!

      g.push :true
      g.gif bottom

      g.goto top

      bottom.set!
      g.push :nil

      dunno2.set!

      g.pop_modifiers
    end
  end

  relates <<-ruby do
      while false do
      end
    ruby

    parse do
      [:while, [:false], nil, true]
    end

    compile do |g|
      top    = g.new_label
      dunno1 = g.new_label
      dunno2 = g.new_label
      bottom = g.new_label

      g.push_modifiers

      top.set!
      g.push :false
      g.gif bottom

      dunno1.set!
      g.push :nil
      g.pop

      g.goto top

      bottom.set!
      g.push :nil

      dunno2.set!

      g.pop_modifiers
    end
  end

  relates <<-ruby do
      begin
        (1 + 2)
        (3 + 4)
      end while false
    ruby

    parse do
      [:while,
       [:false],
       [:block,
        [:call, [:lit, 1], :+, [:arglist, [:lit, 2]]],
        [:call, [:lit, 3], :+, [:arglist, [:lit, 4]]]],
       false]
    end

    compile do |g|
      top    = g.new_label
      dunno1 = g.new_label
      dunno2 = g.new_label
      bottom = g.new_label

      g.push_modifiers

      top.set!
      g.push 1
      g.push 2
      g.send :+, 1, false
      g.pop

      g.push 3
      g.push 4
      g.send :+, 1, false
      g.pop

      dunno1.set!
      g.push :false
      g.gif bottom

      g.goto top

      bottom.set!
      g.push :nil

      dunno2.set!

      g.pop_modifiers
    end
  end

  relates <<-ruby do
      begin
        (1 + 1)
      end while false
    ruby

    parse do
      [:while, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], false]
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
      g.send :+, 1, false
      g.pop

      dunno1.set!
      g.push :false
      g.gif bottom

      g.goto top

      bottom.set!
      g.push :nil

      dunno2.set!

      g.pop_modifiers
    end
  end
end
