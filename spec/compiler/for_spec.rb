require File.dirname(__FILE__) + '/../spec_helper'

describe "A For node" do
  relates <<-ruby do
      for o in ary do
        puts(o)
      end
    ruby

    parse do
      [:for,
       [:call, nil, :ary, [:arglist]],
       [:lasgn, :o],
       [:call, nil, :puts, [:arglist, [:lvar, :o]]]]
    end

    compile do |g|
      g.push :self
      g.send :ary, 0, true

      in_block_send :each, 1.0, 0, false, 1 do |d|
        d.push :self
        d.push_local 0
        d.send :puts, 1, true
      end
    end
  end

  relates <<-ruby do
      for i in (0..max) do
        # do nothing
      end
    ruby

    parse do
      [:for, [:dot2, [:lit, 0], [:call, nil, :max, [:arglist]]], [:lasgn, :i]]
    end

    compile do |g|
      g.push_cpath_top
      g.find_const :Range
      g.push 0
      g.push :self
      g.send :max, 0, true
      g.send :new, 2

      in_block_send :each, 1.0, 0, false, 1 do |d|
        d.push :nil
      end
    end
  end
end
