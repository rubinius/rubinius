require File.expand_path('../../spec_helper', __FILE__)

describe "A Yield node" do
  relates "yield(42, 24)" do
    compile do |g|
      g.push 42
      g.push 24
      g.yield_stack 2
    end
  end

  relates "yield([42, 24])" do
    compile do |g|
      g.push 42
      g.push 24
      g.make_array 2
      g.yield_stack 1
    end
  end

  relates "yield([42])" do
    compile do |g|
      g.push 42
      g.make_array 1
      g.yield_stack 1
    end
  end

  relates "yield([])" do
    compile do |g|
      g.make_array 0
      g.yield_stack 1
    end
  end

  relates "yield(42)" do
    compile do |g|
      g.push 42
      g.yield_stack 1
    end
  end

  relates "yield" do
    compile do |g|
      g.yield_stack 0
    end
  end

  relates "yield([*[]])" do
    compile do |g|
      g.make_array 0
      g.yield_stack 1
    end
  end

  relates "yield([*[1]])" do
    compile do |g|
      g.push 1
      g.make_array 1
      g.yield_stack 1
    end
  end

  relates "yield([*[1, 2]])" do
    compile do |g|
      g.push 1
      g.push 2
      g.make_array 2
      g.yield_stack 1
    end
  end

  relates "yield(*[1])" do
    compile do |g|
      g.push 1
      g.make_array 1
      g.cast_array

      g.yield_splat 0
    end
  end

  relates "yield(*[1, 2])" do
    compile do |g|

      g.push 1
      g.push 2
      g.make_array 2
      g.cast_array

      g.yield_splat 0
    end
  end
end
