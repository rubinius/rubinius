require File.dirname(__FILE__) + '/../spec_helper'

describe "A Match2 node" do
  relates '/x/ =~ "blah"' do
    parse do
      [:match2, [:regex, "x", 0], [:str, "blah"]]
    end

    compile do |g|
      g.memoize do
        g.push_const :Regexp
        g.push_literal "x"
        g.push 0
        g.send :new, 2
      end

      g.push_literal "blah"
      g.string_dup
      g.send :=~, 1
    end
  end

  relates <<-ruby do
      x = "x"
      /\#{x}/ =~ x
    ruby

    parse do
      [:block,
       [:lasgn, :x, [:str, "x"]],
       [:match2, [:dregx, "", [:evstr, [:lvar, :x]]], [:lvar, :x]]]
    end

    compile do |g|
      g.push_literal "x"
      g.string_dup
      g.set_local 0
      g.pop

      g.push_const :Regexp
      g.push_local 0
      g.send :to_s, 0, true
      g.push_literal ""
      g.string_dup
      g.string_append
      g.push 0
      g.send :new, 2

      g.push_local 0
      g.send :=~, 1
    end
  end
end
