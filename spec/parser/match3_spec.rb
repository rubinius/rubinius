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
end
