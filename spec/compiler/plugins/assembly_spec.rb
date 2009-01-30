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

    # TODO: refactor after fixing Iter specs
    compile do |g|
      iter = description do |d|
        d.cast_for_single_block_arg
        d.set_local_depth 0, 0
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :self
        d.send :push_context, 0, true
        d.pop_modifiers
        d.ret
      end
      iter.name = :__block__
      iter.required = 1

      g.passed_block do
        g.push_const :Rubinius
        g.push :self
        g.send :a, 0, true
        g.create_block iter
        g.send_with_block :asm, 1, false
      end
    end

    compile :assembly do |g|
      g.push_context
    end
  end
end
