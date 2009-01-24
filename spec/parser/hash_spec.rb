require File.dirname(__FILE__) + '/../spec_helper'

describe "A Hash node" do
  relates "{ 1 => 2, 3 => 4 }" do
    parse do
      [:hash, [:lit, 1], [:lit, 2], [:lit, 3], [:lit, 4]]
    end

    compile do |g|
      g.push_cpath_top
      g.find_const :Hash
      g.push 1
      g.push 2
      g.push 3
      g.push 4
      g.send :[], 4
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

      g.in_rescue :StandardError do |section|
        case section
        when :body then
          g.push 2
        when :StandardError then
          g.push 3
        end
      end

      g.send :[], 2
    end
  end
end
