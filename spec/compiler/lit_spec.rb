require File.expand_path('../../spec_helper', __FILE__)

describe "A Lit node" do
  relates "0b1111" do
    compile do |g|
      g.push 15
    end
  end

  relates "1.1" do
    compile do |g|
      g.push_literal 1.1
    end
  end

  relates "0xffffffffffffffff" do
    compile do |g|
      g.push_literal 18446744073709551615
    end
  end

  relates "0xffff_ffff_ffff_ffff" do
    compile do |g|
      g.push_literal 18446744073709551615
    end
  end

  relates "07654" do
    compile do |g|
      g.push 4012
    end
  end

  relates "-1" do
    compile do |g|
      g.push(-1)
    end
  end

  relates "1" do
    compile do |g|
      g.push 1
    end
  end

  relates "8000_0000" do
    compile do |g|
      g.push 80000000
    end
  end

  relates ":x" do
    compile do |g|
      g.push_literal :x
    end
  end

  relates ":\"*args\"" do
    compile do |g|
      g.push_literal :"*args"
    end
  end
end
