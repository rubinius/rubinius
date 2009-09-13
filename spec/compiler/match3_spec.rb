require File.dirname(__FILE__) + '/../spec_helper'

describe "A Match3 node" do
  relates '"blah" =~ /x/' do
    parse do
      [:match3, [:regex, "x", 0], [:str, "blah"]]
    end

    compile do |g|
      g.push_literal "blah"
      g.string_dup

      g.memoize do
        g.push_const :Regexp
        g.push_literal "x"
        g.push 0
        g.send :new, 2
      end

      g.send :=~, 1
    end
  end

  relates <<-ruby do
      a = 'abc'
      if a =~ /\#{a}/
        1
      end
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:str, "abc"]],
       [:if,
        [:match3, [:dregx, "", [:evstr, [:lvar, :a]]], [:lvar, :a]],
        [:lit, 1],
        nil]]
    end

    compile do |g|
      g.push_literal "abc"
      g.string_dup
      g.set_local 0
      g.pop
      g.push_local 0

      g.push_const :Regexp
      g.push_local 0
      g.send :to_s, 0, true
      g.push_literal ""
      g.string_dup
      g.string_append

      g.push 0
      g.send :new, 2

      false_label = g.new_label
      done = g.new_label
      g.send :=~, 1
      g.gif false_label
      g.push 1
      g.goto done

      false_label.set!
      g.push :nil
      done.set!
    end
  end
end
