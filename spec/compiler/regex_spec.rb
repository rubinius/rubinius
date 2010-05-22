require File.dirname(__FILE__) + '/../spec_helper'

describe "A Regex node" do
  relates "str.split(//i)" do
    compile do |g|
      g.push :self
      g.send :str, 0, true

      g.memoize do
        g.push_cpath_top
        g.find_const :Regexp
        g.push_literal ""
        g.push 1
        g.send :new, 2
      end

      g.send :split, 1, false
    end
  end

  relates "/x/n" do
    compile do |g|
      g.memoize do
        g.push_cpath_top
        g.find_const :Regexp
        g.push_literal "x"
        g.push 16
        g.send :new, 2
      end
    end
  end

  relates "/x/o" do
    compile do |g|
      g.memoize do
        g.push_cpath_top
        g.find_const :Regexp
        g.push_literal "x"
        g.push 0
        g.send :new, 2
      end
    end
  end

  relates "/x/" do
    compile do |g|
      g.memoize do
        g.push_cpath_top
        g.find_const :Regexp
        g.push_literal "x"
        g.push 0
        g.send :new, 2
      end
    end
  end
end
