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

    # for
  end

  relates <<-ruby do
      for i in (0..max) do
        # do nothing
      end
    ruby

    parse do
      [:for, [:dot2, [:lit, 0], [:call, nil, :max, [:arglist]]], [:lasgn, :i]]
    end

    # for no body
  end
end
