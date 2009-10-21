require File.dirname(__FILE__) + '/../spec_helper'

describe "A Hash node" do
  relates "{ 1 => 2, 3 => 4 }" do
    parse do
      [:hash, [:lit, 1], [:lit, 2], [:lit, 3], [:lit, 4]]
    end

    compile do |g|
      g.push_cpath_top
      g.find_const :Hash
      g.push 2
      g.send :new_from_literal, 1

      g.dup
      g.push 1
      g.push 2
      g.send :[]=, 2
      g.pop

      g.dup
      g.push 3
      g.push 4
      g.send :[]=, 2
      g.pop
    end
  end

  relates "{ 1 => (2 rescue 3) }" do
    parse do
      [:hash, [:lit, 1], [:rescue, [:lit, 2], [:resbody, [:array], [:lit, 3]]]]
    end

    compile do |g|
      g.push_cpath_top
      g.find_const :Hash
      g.push 1
      g.send :new_from_literal, 1

      g.dup
      g.push 1

      g.in_rescue :StandardError do |section|
        case section
        when :body then
          g.push 2
        when :StandardError then
          g.push 3
        end
      end

      g.send :[]=, 2
      g.pop
    end
  end

  relates "{ 1 => [*1] }" do
    parse do
      [:hash, [:lit, 1], [:array, [:splat, [:lit, 1]]]]
    end

    compile do |g|
      g.push_cpath_top
      g.find_const :Hash
      g.push 1
      g.send :new_from_literal, 1

      g.dup
      g.push 1
      g.push 1
      g.cast_array
      g.send :[]=, 2
      g.pop
    end
  end

  relates <<-ruby do
      a = 1
      { :a => a }
    ruby

    parse do
      [:block,
        [:lasgn, :a, [:lit, 1]],
        [:hash, [:lit, :a], [:lvar, :a]]]
    end

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop
      g.push_cpath_top
      g.find_const :Hash
      g.push 1
      g.send :new_from_literal, 1

      g.dup
      g.push_literal :a
      g.push_local 0
      g.send :[]=, 2
      g.pop
    end
  end
end
