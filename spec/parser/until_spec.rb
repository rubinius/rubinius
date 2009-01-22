require File.dirname(__FILE__) + '/../spec_helper'

describe "An Until node" do
  relates <<-ruby do
      begin
        (1 + 1)
      end until false
    ruby

    parse do
      [:until, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], false]
    end

    # until post
  end

  relates "(1 + 1) until false" do
    parse do
      [:until, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    # until pre mod
  end

  relates <<-ruby do
      until false do
        (1 + 1)
      end
    ruby

    parse do
      [:until, [:false], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    # until pre
  end

  relates <<-ruby do
      while not true do
        (1 + 1)
      end
    ruby

    parse do
      [:until, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    # while pre not
  end

  relates "(1 + 1) while not true" do
    parse do
      [:until, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], true]
    end

    # while pre not mod
  end

  relates <<-ruby do
      begin
        (1 + 1)
      end while not true
    ruby

    parse do
      [:until, [:true], [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]], false]
    end

    # while post not
  end
end
