require File.dirname(__FILE__) + '/../spec_helper'

describe "A While node" do
  relates <<-ruby do
      while false do
        (1 + 1)
      end
    ruby

    parse do
      [:while, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    # while pre
  end

  relates <<-ruby do
      begin
        (1 + 1)
      end until not true
    ruby

    parse do
      [:while, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], false]
    end

    # until post not
  end

  relates "(1 + 1) until not true" do
    parse do
      [:while, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    # until pre not mod
  end

  relates <<-ruby do
      until not true do
        (1 + 1)
      end
    ruby

    parse do
      [:while, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    # until pre not
  end

  relates "(1 + 1) while false" do
    parse do
      [:while, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    # while pre mod
  end

  relates <<-ruby do
      while false do
      end
    ruby

    parse do
      [:while, [:false], nil, true]
    end

    # while pre nil
  end

  relates <<-ruby do
      begin
        (1 + 2)
        (3 + 4)
      end while false
    ruby

    parse do
      [:while,
       [:false],
       [:block,
        [:call, [:lit, 1], :+, [:arglist, [:lit, 2]]],
        [:call, [:lit, 3], :+, [:arglist, [:lit, 4]]]],
       false]
    end

    # while post2
  end

  relates <<-ruby do
      begin
        (1 + 1)
      end while false
    ruby

    parse do
      [:while, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], false]
    end

    # while post
  end
end
