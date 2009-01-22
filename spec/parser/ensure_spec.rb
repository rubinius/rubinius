require File.dirname(__FILE__) + '/../spec_helper'

describe "An Ensure node" do
  relates <<-ruby do
      begin
        # do nothing
      rescue
        # do nothing
      ensure
        # do nothing
      end
    ruby

    parse do
      [:ensure, [:rescue, [:resbody, [:array], nil]], [:nil]]
    end

    # begin_rescue_ensure_all_empty
  end

  relates <<-ruby do
      begin
        (1 + 1)
      rescue SyntaxError => e1
        2
      rescue Exception => e2
        3
      else
        4
      ensure
        5
      end
    ruby

    parse do
      [:ensure,
         [:rescue,
          [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]],
          [:resbody,
           [:array, [:const, :SyntaxError], [:lasgn, :e1, [:gvar, :$!]]],
           [:lit, 2]],
          [:resbody,
           [:array, [:const, :Exception], [:lasgn, :e2, [:gvar, :$!]]],
           [:lit, 3]],
          [:lit, 4]],
         [:lit, 5]]
    end

    # ensure
  end

  relates <<-ruby do
      begin
        a
      rescue
        # do nothing
      ensure
        # do nothing
      end
    ruby

    parse do
      [:ensure,
         [:rescue, [:call, nil, :a, [:arglist]], [:resbody, [:array], nil]],
         [:nil]]
    end

    # begin_rescue_ensure 
  end

  relates <<-ruby do
      begin
        a
      rescue => mes
        # do nothing
      end

      begin
        b
      rescue => mes
        # do nothing
      end
    ruby

    parse do
      [:block,
         [:rescue,
          [:call, nil, :a, [:arglist]],
          [:resbody, [:array, [:lasgn, :mes, [:gvar, :$!]]], nil]],
         [:rescue,
          [:call, nil, :b, [:arglist]],
          [:resbody, [:array, [:lasgn, :mes, [:gvar, :$!]]], nil]]]
    end

    # begin_rescue_twice
  end
end
