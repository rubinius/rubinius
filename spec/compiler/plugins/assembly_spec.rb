require File.dirname(__FILE__) + '/../../spec_helper'

describe "A Call node using InlineAssembly plugin" do
  relates <<-ruby do
      Rubinius.asm(a) do |m|
        push 1
      end
    ruby

    parse do
      [:iter,
       [:call,
        [:const, :Rubinius],
        :asm,
        [:arglist, [:call, nil, :a, [:arglist]]]],
       [:lasgn, :m],
       [:call, nil, :push, [:arglist, [:lit, 1]]]]
    end

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

        iter = description do |i|
          i.cast_for_multi_block_arg
          i.cast_array
          i.shift_array

          i.set_local_depth 1, 0
          i.pop
          i.shift_array
          i.set_local_depth 1, 1
          i.pop
          i.pop

          i.push_modifiers
          i.new_label.set!

          i.push :self
          i.push_local_depth 1, 1
          i.send :run, 1, true
          i.pop

          i.push :self
          i.push_local_depth 1, 0
          i.send :run, 1, true
          i.pop

          i.push :self
          i.send :push_block, 0, true
          i.pop
          i.push :self
          i.push_literal :initialize
          i.push 0
          i.push :true
          i.send :send_with_splat, 3, true

          i.pop_modifiers
          i.ret
        end

        d.create_block iter
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

        iter = description do |i|
          i.cast_for_single_block_arg

          i.set_local_depth 1, 1
          i.pop

          i.push_modifiers
          i.new_label.set!

          i.push :self
          i.send :push_block, 0, true
          i.pop

          i.push :self
          i.push_local_depth 1, 1
          i.send :run, 1, true
          i.pop

          i.push :self
          i.push_literal :initialize
          i.push 0
          i.push :true
          i.send :send_with_block, 3, true

          i.pop_modifiers
          i.ret
        end

        d.create_block iter
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
