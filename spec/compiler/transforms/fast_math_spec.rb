require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using FastMath transform" do
  relates "1 + 1" do
    compile do |g|
      g.push 1
      g.push 1
      g.send :+, 1, false
    end

    compile :fast_math do |g|
      g.push 1
      g.push 1
      g.meta_send_op_plus :+
    end
  end

  relates "1.+(*a)" do
    compile do |g|
      g.push 1
      g.push :self
      g.send :a, 0, true
      g.cast_array
      g.push :nil
      g.send_with_splat :+, 0, false, false
    end

    compile :fast_math do |g|
      g.push 1
      g.push :self
      g.send :a, 0, true
      g.cast_array
      g.push :nil
      g.send_with_splat :+, 0, false, false
    end
  end

  relates "1 - 1" do
    compile do |g|
      g.push 1
      g.push 1
      g.send :-, 1, false
    end

    compile :fast_math do |g|
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

    compile :fast_math do |g|
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

    compile :fast_math do |g|
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

    compile :fast_math do |g|
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

    compile :fast_math do |g|
      g.push 1
      g.push 1
      g.meta_send_op_gt :>
    end
  end
end
