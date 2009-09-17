require File.dirname(__FILE__) + '/../spec_helper'

describe "A Lit node" do
  relates "0b1111" do
    parse do
      [:lit, 15]
    end

    compile do |g|
      g.push 15
    end
  end

  relates "1.1" do
    parse do
      [:lit, 1.1]
    end

    compile do |g|
      g.push_literal 1.1
    end
  end

  relates "0xffffffffffffffff" do
    parse do
      [:lit, 18446744073709551615]
    end

    compile do |g|
      g.push_literal 18446744073709551615
    end
  end

  relates "0xffff_ffff_ffff_ffff" do
    parse do
      [:lit, 18446744073709551615]
    end

    compile do |g|
      g.push_literal 18446744073709551615
    end
  end

  relates "07654" do
    parse do
      [:lit, 4012]
    end

    compile do |g|
      g.push 4012
    end
  end

  relates "-1" do
    parse do
      [:lit, -1]
    end

    compile do |g|
      g.push(-1)
    end
  end

  relates "1" do
    parse do
      [:lit, 1]
    end

    compile do |g|
      g.push 1
    end
  end

  relates "8000_0000" do
    parse do
      [:lit, 80000000]
    end

    compile do |g|
      g.push 80000000
    end
  end

  relates ":x" do
    parse do
      [:lit, :x]
    end

    compile do |g|
      g.push_literal :x
    end
  end

  relates ":\"*args\"" do
    parse do
      [:lit, :"*args"]
    end

    compile do |g|
      g.push_literal :"*args"
    end
  end
end
