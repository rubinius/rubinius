require File.dirname(__FILE__) + '/../spec_helper'

describe "An Array node" do
  relates '[1, :b, "c"]' do
    parse do
      [:array, [:lit, 1], [:lit, :b], [:str, "c"]]
    end

    # array
  end

  relates "%w[a b c]" do
    parse do
      [:array, [:str, "a"], [:str, "b"], [:str, "c"]]
    end

    # array_pct_W
  end

  relates "%w[a \#{@b} c]" do
    parse do
      [:array, [:str, "a"], [:str, "\#{@b}"], [:str, "c"]]
    end

    # array_pct_W_dstr
  end

  relates "[*[1]]" do
    parse do
      [:array, [:splat, [:array, [:lit, 1]]]]
    end

    # splat array
  end

  relates "[*1]" do
    parse do
      [:array, [:splat, [:lit, 1]]]
    end

    # splat lit 1
  end

  relates "[1, *2]" do
    parse do
      [:array, [:lit, 1], [:splat, [:lit, 2]]]
    end

    # splat lit n
  end
end
