require File.dirname(__FILE__) + '/../spec_helper'

describe "A Regex node" do
  relates "str.split(//i)" do
    parse do
      [:call,
        [:call, nil, :str, [:arglist]],
        :split,
        [:arglist, [:regex, "", 1]]]
    end

    compile do |g|
      g.push :self
      g.send :str, 0, true

      g.memoize do
        g.push_const :Regexp
        g.push_literal ""
        g.push 1
        g.send :new, 2
      end

      g.send :split, 1, false
    end
  end

  relates "/x/n" do
    parse do
      [:regex, "x", 16]
    end

    compile do |g|
      g.memoize do
        g.push_const :Regexp
        g.push_literal "x"
        g.push 16
        g.send :new, 2
      end
    end
  end

  relates "/x/o" do
    parse do
      [:regex, "x", 0]
    end

    compile do |g|
      g.memoize do
        g.push_const :Regexp
        g.push_literal "x"
        g.push 0
        g.send :new, 2
      end
    end
  end

  relates "/x/" do
    parse do
      [:regex, "x", 0]
    end

    compile do |g|
      g.memoize do
        g.push_const :Regexp
        g.push_literal "x"
        g.push 0
        g.send :new, 2
      end
    end
  end
end
