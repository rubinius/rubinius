require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dsym node" do
  relates ':"x#{(1 + 1)}y"' do
    compile do |g|
      g.push_literal "y"
      g.string_dup

      g.push 1
      g.push 1
      g.send :+, 1, false
      g.send :to_s, 0, true

      g.push_literal "x"
      g.string_dup

      g.string_append
      g.string_append

      g.send :to_sym, 0, true
    end
  end
end
