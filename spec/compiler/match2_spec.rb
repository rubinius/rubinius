require File.dirname(__FILE__) + '/../spec_helper'

describe "A Match2 node" do
  relates '/x/ =~ "blah"' do
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

    compile do |g|
      g.push_literal "x"
      g.string_dup
      g.set_local 0
      g.pop

      g.push_const :Regexp
      g.push_local 0
      g.send :to_s, 0, true
      g.string_build 1

      g.push 0
      g.send :new, 2

      g.push_local 0
      g.send :=~, 1
    end
  end
end
