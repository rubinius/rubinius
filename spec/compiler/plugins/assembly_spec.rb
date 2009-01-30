require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using InlineAssembly plugin" do
  relates <<-ruby do
      Rubinius.asm(a) do |m|
        push_context
      end
    ruby

    parse do
      [:iter,
       [:call,
        [:const, :Rubinius],
        :asm,
        [:arglist, [:call, nil, :a, [:arglist]]]],
       [:lasgn, :m],
       [:call, nil, :push_context, [:arglist]]]
    end

    compile do |g|
      g.passed_block do
        g.push_const :Rubinius
        g.push :self
        g.send :a, 0, true

        g.in_block_send :asm, :single, nil, 1, false do |d|
          d.push :self
          d.send :push_context, 0, true
        end
      end
    end

    compile :assembly do |g|
      g.push_context
    end
  end
end
