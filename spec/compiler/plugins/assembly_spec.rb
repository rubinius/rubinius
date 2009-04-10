require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using InlineAssembly plugin" do
  relates <<-ruby do
      Rubinius.asm(a) do |m|
        push 1
      end
    ruby

    parse do
      [:iter,
       [:call,
        [:const, :Rubinius],
        :asm,
        [:arglist, [:call, nil, :a, [:arglist]]]],
       [:lasgn, :m],
       [:call, nil, :push, [:arglist, [:lit, 1]]]]
    end

    compile do |g|
      g.push_const :Rubinius
      g.push :self
      g.send :a, 0, true

      g.in_block_send :asm, :single, nil, 1, false do |d|
        d.push :self
        d.push 1
        d.send :push, 1, true
      end
    end

    compile :assembly do |g|
      g.push 1
    end
  end
end
