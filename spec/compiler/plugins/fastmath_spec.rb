require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using FastMath plugin" do
  relates "1 + 1" do
    compile do |g|
      g.push 1
      g.push 1
      g.send :+, 1, false
    end

    compile :fastmath do |g|
      g.push 1
      g.push 1
      g.meta_send_op_plus :+
    end
  end

  relates "1 - 1" do
    compile do |g|
      g.push 1
      g.push 1
      g.send :-, 1, false
    end

    compile :fastmath do |g|
      g.push 1
      g.push 1
      g.meta_send_op_minus :-
    end
  end

  relates "1 == 1" do
    compile do |g|
      g.push 1
      g.push 1
      g.send :==, 1, false
    end

    compile :fastmath do |g|
      g.push 1
      g.push 1
      g.meta_send_op_equal :==
    end
  end

  relates "1 === 1" do
    compile do |g|
      g.push 1
      g.push 1
      g.send :===, 1, false
    end

    compile :fastmath do |g|
      g.push 1
      g.push 1
      g.meta_send_op_tequal :===
    end
  end

  relates "1 < 1" do
    compile do |g|
      g.push 1
      g.push 1
      g.send :<, 1, false
    end

    compile :fastmath do |g|
      g.push 1
      g.push 1
      g.meta_send_op_lt :<
    end
  end

  relates "1 > 1" do
    compile do |g|
      g.push 1
      g.push 1
      g.send :>, 1, false
    end

    compile :fastmath do |g|
      g.push 1
      g.push 1
      g.meta_send_op_gt :>
    end
  end
end
