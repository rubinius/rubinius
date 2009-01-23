require File.dirname(__FILE__) + '/../spec_helper'

describe "A Return node" do
  relates "return" do
    parse do
      [:return]
    end

    compile do |g|
      g.push :nil
      g.ret
    end
  end

  relates "return 1" do
    parse do
      [:return, [:lit, 1]]
    end

    compile do |g|
      g.push 1
      g.ret
    end
  end

  relates "return *1" do
    parse do
      [:return, [:svalue, [:splat, [:lit, 1]]]]
    end

    compile do |g|
      bottom = g.new_label

      g.push 1
      g.cast_array
      g.dup
      g.send :size, 0
      g.push 1
      g.send :>, 1
      g.git bottom

      g.push 0
      g.send :at, 1

      bottom.set!

      g.ret
    end
  end

  relates "return 1, 2, 3" do
    parse do
      [:return, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]
    end

    compile do |g|
      g.push 1
      g.push 2
      g.push 3
      g.make_array 3
      g.ret
    end
  end

  relates "return [*[1]]" do
    parse do
      [:return, [:array, [:splat, [:array, [:lit, 1]]]]]
    end

    compile do |g|
      g.array_of_splatted_array
      g.ret
    end
  end

  relates "return *[1]" do
    parse do
      [:return, [:svalue, [:splat, [:array, [:lit, 1]]]]]
    end

    compile do |g|
      g.splatted_array
      g.ret
    end
  end
end
