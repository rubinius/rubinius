require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using FastNew transform" do
  relates "new" do
    compile do |g|
      g.push :self
      g.send :new, 0, true
    end

    compile :fast_new do |g|
      slow = g.new_label
      done = g.new_label

      g.push :self
      g.dup
      g.check_serial_private :new, Rubinius::CompiledMethod::KernelMethodSerial
      gif slow

      g.send :allocate, 0, true
      g.dup
      g.send :initialize, 0, true
      g.pop
      g.goto done

      slow.set!

      g.send :new, 0, true

      done.set!
    end
  end

  relates "new(a)" do
    compile do |g|
      g.push :self
      g.push :self
      g.send :a, 0, true
      g.send :new, 1, true
    end

    compile :fast_new do |g|
      slow = g.new_label
      done = g.new_label

      g.push :self
      g.dup
      g.check_serial_private :new, Rubinius::CompiledMethod::KernelMethodSerial
      gif slow

      g.send :allocate, 0, true
      g.dup
      g.push :self
      g.send :a, 0, true
      g.send :initialize, 1, true
      g.pop
      g.goto done

      slow.set!
      g.push :self
      g.send :a, 0, true
      g.send :new, 1, true

      done.set!
    end
  end

  relates "A.new" do
    compile do |g|
      g.push_const :A
      g.send :new, 0, false
    end

    compile :fast_new do |g|
      slow = g.new_label
      done = g.new_label

      g.push_const :A
      g.dup
      g.check_serial :new, Rubinius::CompiledMethod::KernelMethodSerial
      gif slow

      g.send :allocate, 0, true
      g.dup
      g.send :initialize, 0, true
      g.pop
      g.goto done

      slow.set!
      g.send :new, 0, false

      done.set!
    end
  end

  relates "A.new(a)" do
    compile do |g|
      g.push_const :A
      g.push :self
      g.send :a, 0, true
      g.send :new, 1, false
    end

    compile :fast_new do |g|
      slow = g.new_label
      done = g.new_label

      g.push_const :A
      g.dup
      g.check_serial :new, Rubinius::CompiledMethod::KernelMethodSerial
      gif slow

      g.send :allocate, 0, true
      g.dup
      g.push :self
      g.send :a, 0, true
      g.send :initialize, 1, true
      g.pop
      g.goto done

      slow.set!
      g.push :self
      g.send :a, 0, true
      g.send :new, 1, false

      done.set!
    end
  end
end
