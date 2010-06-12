require File.expand_path('../../../spec_helper', __FILE__)

describe "A Call node using InlineAssembly transform" do
  relates <<-ruby do
      Rubinius.asm(a) do |m|
        push 1
      end
    ruby

    compile do |g|
      g.push_const :Rubinius
      g.push :self
      g.send :a, 0, true

      g.in_block_send :asm, :single, nil, 1, false do |d|
        d.push :self
        d.push 1
        d.send :push, 1, true
      end
    end

    compile :assembly do |g|
      g.push 1
    end
  end

  relates <<-ruby do
      def new(*args)
        obj = allocate()

        Rubinius.asm(args, obj) do |args, obj|
          run obj
          run args
          push_block
          send_with_splat :initialize, 0, true
        end

        return obj
      end
    ruby

    compile do |g|
      in_method :new do |d|
        d.push :self
        d.send :allocate, 0, true
        d.set_local 1
        d.pop

        d.push_const :Rubinius
        d.push_local 0
        d.push_local 1

        e = new_block_generator(g)

        e.cast_for_multi_block_arg
        e.cast_array
        e.shift_array

        e.set_local_depth 1, 0
        e.pop
        e.shift_array
        e.set_local_depth 1, 1
        e.pop
        e.pop

        e.push_modifiers
        e.new_label.set!

        e.push :self
        e.push_local_depth 1, 1
        e.send :run, 1, true
        e.pop

        e.push :self
        e.push_local_depth 1, 0
        e.send :run, 1, true
        e.pop

        e.push :self
        e.send :push_block, 0, true
        e.pop
        e.push :self
        e.push_literal :initialize
        e.push 0
        e.push :true
        e.send :send_with_splat, 3, true

        e.pop_modifiers
        e.ret

        d.create_block(e)
        d.send_with_block :asm, 2, false
        d.pop
        d.push_local 1
        d.ret
      end
    end

    compile :assembly do |g|
      in_method :new do |d|
        d.push :self
        d.send :allocate, 0, true
        d.set_local 1
        d.pop

        d.push_local 1
        d.push_local 0
        d.push_block
        d.send_with_splat :initialize, 0, true
        d.pop
        d.push_local 1
        d.ret
      end
    end
  end

  relates <<-ruby do
      def new(cnt)
        obj = allocate_sized cnt
        Rubinius.asm(obj) do |obj|
          push_block
          run obj
          send_with_block :initialize, 0, true
        end

        return obj
      end
    ruby

    compile do |g|
      in_method :new do |d|
        d.push :self
        d.push_local 0
        d.send :allocate_sized, 1, true
        d.set_local 1
        d.pop

        d.push_const :Rubinius
        d.push_local 1

        e = new_block_generator(g)

        e.cast_for_single_block_arg

        e.set_local_depth 1, 1
        e.pop

        e.push_modifiers
        e.new_label.set!

        e.push :self
        e.send :push_block, 0, true
        e.pop

        e.push :self
        e.push_local_depth 1, 1
        e.send :run, 1, true
        e.pop

        e.push :self
        e.push_literal :initialize
        e.push 0
        e.push :true
        e.send :send_with_block, 3, true

        e.pop_modifiers
        e.ret

        d.create_block(e)
        d.send_with_block :asm, 1, false
        d.pop
        d.push_local 1
        d.ret
      end
    end

    compile :assembly do |g|
      in_method :new do |d|
        d.push :self
        d.push_local 0
        d.send :allocate_sized, 1, true
        d.set_local 1
        d.pop

        d.push_block
        d.push_local 1
        d.send_with_block :initialize, 0, true
        d.pop
        d.push_local 1
        d.ret
      end
    end
  end
end
