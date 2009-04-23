require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using FastNew plugin" do
  relates "new" do
    parse do
      [:call, nil, :new, [:arglist]]
    end

    compile do |g|
      g.push :self
      g.send :new, 0, true
    end

    compile :fastnew do |g|
      slow = g.new_label
      done = g.new_label

      g.push :self
      g.dup
      idx = g.check_serial idx, Rubinius::CompiledMethod::KernelMethodSerial
      gif slow

      g.send :allocate, 0
      g.dup
      g.send :initialize, 0, true
      g.pop
      g.goto done

      slow.set!
      g.allow_private
      g.send_method idx

      done.set!
    end
  end

  relates "new(a)" do
    parse do
      [:call, nil, :new, [:arglist, [:call, nil, :a, [:arglist]]]]
    end

    compile do |g|
      g.push :self
      g.push :self
      g.send :a, 0, true
      g.send :new, 1, true
    end

    compile :fastnew do |g|
      slow = g.new_label
      done = g.new_label

      g.push :self
      g.dup
      idx = g.check_serial idx, Rubinius::CompiledMethod::KernelMethodSerial
      gif slow

      g.send :allocate, 0
      g.dup
      g.push :self
      g.send :a, 0, true
      g.send :initialize, 1, true
      g.pop
      g.goto done

      slow.set!
      g.push :self
      g.send :a, 0, true
      g.allow_private
      g.send_stack idx, 1

      done.set!
    end
  end

  relates "A.new" do
    parse do
      [:call, [:const, :A], :new, [:arglist]]
    end

    compile do |g|
      g.push_const :A
      g.send :new, 0, false
    end

    compile :fastnew do |g|
      slow = g.new_label
      done = g.new_label

      g.push_const :A
      g.dup
      idx = g.check_serial idx, Rubinius::CompiledMethod::KernelMethodSerial
      gif slow

      g.send :allocate, 0
      g.dup
      g.send :initialize, 0, true
      g.pop
      g.goto done

      slow.set!
      g.allow_private
      g.send_method idx

      done.set!
    end
  end

  relates "A.new(a)" do
    parse do
      [:call, [:const, :A], :new, [:arglist, [:call, nil, :a, [:arglist]]]]
    end

    compile do |g|
      g.push_const :A
      g.push :self
      g.send :a, 0, true
      g.send :new, 1, false
    end

    compile :fastnew do |g|
      slow = g.new_label
      done = g.new_label

      g.push_const :A
      g.dup
      idx = g.check_serial idx, Rubinius::CompiledMethod::KernelMethodSerial
      gif slow

      g.send :allocate, 0
      g.dup
      g.push :self
      g.send :a, 0, true
      g.send :initialize, 1, true
      g.pop
      g.goto done

      slow.set!
      g.push :self
      g.send :a, 0, true
      g.allow_private
      g.send_stack idx, 1

      done.set!
    end
  end
end
