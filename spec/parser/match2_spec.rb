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
end
