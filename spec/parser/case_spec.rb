require File.dirname(__FILE__) + '/../spec_helper'

describe "A Case node" do
  relates <<-ruby do
      var = 2
      result = ""
      case var
      when 1 then
        puts("something")
        result = "red"
      when 2, 3 then
        result = "yellow"
      when 4 then
        # do nothing
      else
        result = "green"
      end
      case result
      when "red" then
        var = 1
      when "yellow" then
        var = 2
      when "green" then
        var = 3
      else
        # do nothing
      end
    ruby

    parse do
      [:block,
       [:lasgn, :var, [:lit, 2]],
       [:lasgn, :result, [:str, ""]],
       [:case,
        [:lvar, :var],
        [:when,
         [:array, [:lit, 1]],
         [:block,
          [:call, nil, :puts, [:arglist, [:str, "something"]]],
          [:lasgn, :result, [:str, "red"]]]],
        [:when,
         [:array, [:lit, 2], [:lit, 3]],
         [:lasgn, :result, [:str, "yellow"]]],
        [:when, [:array, [:lit, 4]], nil],
        [:lasgn, :result, [:str, "green"]]],
       [:case,
        [:lvar, :result],
        [:when, [:array, [:str, "red"]], [:lasgn, :var, [:lit, 1]]],
        [:when, [:array, [:str, "yellow"]], [:lasgn, :var, [:lit, 2]]],
        [:when, [:array, [:str, "green"]], [:lasgn, :var, [:lit, 3]]],
        nil]]
    end

    # case
  end

  relates <<-ruby do
      case a
      when b then
        case
        when (d and e) then
          f
        else
          # do nothing
        end
      else
        # do nothing
      end
    ruby

    parse do
      [:case,
       [:call, nil, :a, [:arglist]],
       [:when,
        [:array, [:call, nil, :b, [:arglist]]],
        [:case,
         nil,
         [:when,
          [:array,
           [:and,
            [:call, nil, :d, [:arglist]],
            [:call, nil, :e, [:arglist]]]],
          [:call, nil, :f, [:arglist]]],
         nil]],
       nil]
    end

    # case nested inner no expr
  end

  relates <<-ruby do
      var1 = 1
      var2 = 2
      result = nil
      case var1
      when 1 then
        case var2
        when 1 then
          result = 1
        when 2 then
          result = 2
        else
          result = 3
        end
      when 2 then
        case var2
        when 1 then
          result = 4
        when 2 then
          result = 5
        else
          result = 6
        end
      else
        result = 7
      end
    ruby

    parse do
      [:block,
       [:lasgn, :var1, [:lit, 1]],
       [:lasgn, :var2, [:lit, 2]],
       [:lasgn, :result, [:nil]],
       [:case,
        [:lvar, :var1],
        [:when,
         [:array, [:lit, 1]],
         [:case,
          [:lvar, :var2],
          [:when, [:array, [:lit, 1]], [:lasgn, :result, [:lit, 1]]],
          [:when, [:array, [:lit, 2]], [:lasgn, :result, [:lit, 2]]],
          [:lasgn, :result, [:lit, 3]]]],
        [:when,
         [:array, [:lit, 2]],
         [:case,
          [:lvar, :var2],
          [:when, [:array, [:lit, 1]], [:lasgn, :result, [:lit, 4]]],
          [:when, [:array, [:lit, 2]], [:lasgn, :result, [:lit, 5]]],
          [:lasgn, :result, [:lit, 6]]]],
        [:lasgn, :result, [:lit, 7]]]]
    end

    # case nested
  end

  relates <<-ruby do
      case
      when (a == 1) then
        :a
      when (a == 2) then
        :b
      else
        :c
      end
    ruby

    parse do
      [:case,
       nil,
       [:when,
        [:array,
         [:call, [:call, nil, :a, [:arglist]], :==, [:arglist, [:lit, 1]]]],
        [:lit, :a]],
       [:when,
        [:array,
         [:call, [:call, nil, :a, [:arglist]], :==, [:arglist, [:lit, 2]]]],
        [:lit, :b]],
       [:lit, :c]]
    end

    # case no expr
  end

  relates <<-ruby do
      case a
      when :b, *c then
        d
      else
        e
      end
    ruby

    parse do
      [:case,
       [:call, nil, :a, [:arglist]],
       [:when,
        [:array, [:lit, :b], [:when, [:call, nil, :c, [:arglist]], nil]],
        [:call, nil, :d, [:arglist]]],
       [:call, nil, :e, [:arglist]]]
    end

    # case splat
  end
end
