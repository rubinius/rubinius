require File.dirname(__FILE__) + '/../spec_helper'

describe "A Proc node" do
  proc_no_args = lambda do |g|
    g.push :self
    in_block_send :proc, 0 do |d|
      d.push :self
      d.send :x, 0, true
      d.push 1
      d.meta_send_op_plus
    end
  end

  relates "proc { || (x + 1) }" do
    parse do
      [:iter,
       [:call, nil, :proc, [:arglist]],
       0,
       [:call, [:call, nil, :x, [:arglist]], :+, [:arglist, [:lit, 1]]]]
    end

    compile(&proc_no_args)
  end

  relates "proc { |x| (x + 1) }" do
    parse do
      [:iter,
       [:call, nil, :proc, [:arglist]],
       [:lasgn, :x],
       [:call, [:lvar, :x], :+, [:arglist, [:lit, 1]]]]
    end

    compile do |g|
      g.push :self
      in_block_send :proc, 1 do |d|
        d.push_local_depth 0, 0
        d.push 1
        d.meta_send_op_plus
      end
    end
  end

  relates "proc { |x, y| (x + y) }" do
    parse do
      [:iter,
       [:call, nil, :proc, [:arglist]],
       [:masgn, [:array, [:lasgn, :x], [:lasgn, :y]]],
       [:call, [:lvar, :x], :+, [:arglist, [:lvar, :y]]]]
    end

    compile do |g|
      g.push :self
      in_block_send :proc, 2 do |d|
        d.push_local_depth 0, 0
        d.push_local_depth 0, 1
        d.meta_send_op_plus
      end
    end
  end

  relates "proc { (x + 1) }" do
    parse do
      [:iter,
       [:call, nil, :proc, [:arglist]],
       nil,
       [:call, [:call, nil, :x, [:arglist]], :+, [:arglist, [:lit, 1]]]]
    end

    compile(&proc_no_args)
  end
end
