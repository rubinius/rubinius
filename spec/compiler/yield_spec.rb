require File.dirname(__FILE__) + '/../spec_helper'

describe "A Yield node" do
  relates "yield(42, 24)" do
    parse do
      [:yield, [:lit, 42], [:lit, 24]]
    end

    compile do |g|
      g.push_block
      g.push 42
      g.push 24
      g.meta_send_call 2
    end
  end

  relates "yield([42, 24])" do
    parse do
      [:yield, [:array, [:lit, 42], [:lit, 24]]]
    end

    compile do |g|
      g.push_block
      g.push 42
      g.push 24
      g.make_array 2
      g.meta_send_call 1
    end
  end

  relates "yield([42])" do
    parse do
      [:yield, [:array, [:lit, 42]]]
    end

    compile do |g|
      g.push_block
      g.push 42
      g.make_array 1
      g.meta_send_call 1
    end
  end

  relates "yield([])" do
    parse do
      [:yield, [:array]]
    end

    compile do |g|
      g.push_block
      g.make_array 0
      g.meta_send_call 1
    end
  end

  relates "yield(42)" do
    parse do
      [:yield, [:lit, 42]]
    end

    compile do |g|
      g.push_block
      g.push 42
      g.meta_send_call 1
    end
  end

  relates "yield" do
    parse do
      [:yield]
    end

    compile do |g|
      g.push_block
      g.meta_send_call 0
    end
  end

  relates "yield([*[1]])" do
    parse do
      [:yield, [:array, [:splat, [:array, [:lit, 1]]]]]
    end

    compile do |g|
      g.push_block
      g.array_of_splatted_array

      g.meta_send_call 1
    end
  end

  relates "yield([*[1, 2]])" do
    parse do
      [:yield, [:array, [:splat, [:array, [:lit, 1], [:lit, 2]]]]]
    end

    compile do |g|
      g.push_block
      g.array_of_splatted_array 2 do
        g.push 1
        g.push 2
      end

      g.meta_send_call 1
    end
  end

  relates "yield(*[1])" do
    parse do
      [:yield, [:splat, [:array, [:lit, 1]]]]
    end

    compile do |g|
      g.push_block

      g.push 1
      g.make_array 1
      g.cast_array

      g.push :nil

      g.send_with_splat :call, 0, false, false
    end
  end

  relates "yield(*[1, 2])" do
    parse do
      [:yield, [:splat, [:array, [:lit, 1], [:lit, 2]]]]
    end

    compile do |g|
      g.push_block

      g.push 1
      g.push 2
      g.make_array 2
      g.cast_array

      g.push :nil

      g.send_with_splat :call, 0, false, false
    end
  end
end
