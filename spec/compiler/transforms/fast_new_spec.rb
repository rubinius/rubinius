require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using FastNew transform" do
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
      g.check_serial_private :new, Rubinius::CompiledCode::KernelMethodSerial
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

  relates "new(&block)" do
    fast_new_with_block = lambda do
      g.push :self
      g.push :self
      g.send :block, 0, true
      g.dup
      g.is_nil

      no_block = g.new_label
      g.git no_block

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      no_block.set!
      g.send_with_block :new, 0, true
    end

    compile(&fast_new_with_block)

    compile(:fast_new, &fast_new_with_block)
  end

  relates "new(a, &block)" do
    fast_new_with_arg_block = lambda do
      g.push :self
      g.push :self
      g.send :a, 0, true

      g.push :self
      g.send :block, 0, true
      g.dup
      g.is_nil

      no_block = g.new_label
      g.git no_block

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      no_block.set!
      g.send_with_block :new, 1, true
    end

    compile(&fast_new_with_arg_block)

    compile(:fast_new, &fast_new_with_arg_block)
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
      g.check_serial :new, Rubinius::CompiledCode::KernelMethodSerial
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
      g.check_serial :new, Rubinius::CompiledCode::KernelMethodSerial
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

  relates "A.new(&block)" do
    fast_new_with_block = lambda do
      g.push_const :A
      g.push :self
      g.send :block, 0, true
      g.dup
      g.is_nil

      no_block = g.new_label
      g.git no_block

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      no_block.set!
      g.send_with_block :new, 0, false
    end

    compile(&fast_new_with_block)

    compile(:fast_new, &fast_new_with_block)
  end

  relates "A.new(a, &block)" do
    fast_new_with_arg_block = lambda do
      g.push_const :A
      g.push :self
      g.send :a, 0, true

      g.push :self
      g.send :block, 0, true
      g.dup
      g.is_nil

      no_block = g.new_label
      g.git no_block

      g.push_cpath_top
      g.find_const :Proc
      g.swap
      g.send :__from_block__, 1

      no_block.set!
      g.send_with_block :new, 1, false
    end

    compile(&fast_new_with_arg_block)

    compile(:fast_new, &fast_new_with_arg_block)
  end

  ruby_version_is "1.9" do
    relates "A.new(a { |(b, c)| })" do
      compile :fast_new do
        d = new_block_generator(g)
        d.push_local 0
        d.cast_array
        d.shift_array
        d.set_local 0
        d.pop

        d.shift_array
        d.set_local 1
        d.pop
        d.pop

        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.ret

        slow = g.new_label
        done = g.new_label

        g.push_const :A
        g.dup
        g.check_serial :new, Rubinius::CompiledCode::KernelMethodSerial
        gif slow

        g.send :allocate, 0, true
        g.dup

        g.push :self
        g.create_block(d)
        g.send_with_block :a, 0, true

        g.send :initialize, 1, true
        g.pop
        g.goto done

        slow.set!

        g.push :self
        g.create_block(d)
        g.send_with_block :a, 0, true
        g.send :new, 1, false

        done.set!
      end
    end
  end
end
