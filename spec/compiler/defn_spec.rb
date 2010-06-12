require File.expand_path('../../spec_helper', __FILE__)

describe "A Defn node" do
  relates <<-ruby do
      def m
        begin
        end
      end
    ruby

    compile do |g|
      in_method :m do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def m
        return :a
      ensure
        return :b
      end
    ruby

    compile do |g|
      in_method :m do |d|
        ensure_exc_lbl = d.new_label
        ensure_noexc_lbl = d.new_label

        d.setup_unwind ensure_exc_lbl

        d.new_label.set!
        exc = d.save_exception

        d.push_literal :a
        d.ensure_return
        d.pop_unwind
        d.goto ensure_noexc_lbl

        ensure_exc_lbl.set!
        d.push_exception_state
        d.push_literal :b

        d.restore_exception exc

        d.ret
        d.pop
        d.restore_exception_state
        d.reraise

        ensure_noexc_lbl.set!
        d.push_literal :b
        d.ret
        d.pop
      end
    end
  end

  relates <<-ruby do
      def blah(*args, &block)
        other(42, *args, &block)
      end
    ruby

    compile do |g|
      in_method :blah do |d|
        no_proc  = d.new_label
        no_splat = d.new_label

        d.push_proc
        d.set_local 1
        d.pop

        d.push :self
        d.push 42          # only line different from block_pass_splat
        d.push_local 0
        d.cast_array
        d.push_local 1
        d.dup
        d.is_nil
        d.git no_splat

        d.push_cpath_top
        d.find_const :Proc
        d.swap
        d.send :__from_block__, 1

        no_splat.set!

        d.send_with_splat :other, 1, true, false # ok, and this one
      end
    end
  end

  relates <<-ruby do
      def blah(*args, &block)
        other(*args, &block)
      end
    ruby

    compile do |g|
      g.in_method :blah do |d|
        no_proc  = d.new_label
        no_splat = d.new_label

        d.push_proc
        d.set_local 1
        d.pop

        d.push :self
        d.push_local 0
        d.cast_array
        d.push_local 1
        d.dup
        d.is_nil
        d.git no_splat

        d.push_cpath_top
        d.find_const :Proc # FIX: why push_cpath/find vs push_const ?
        d.swap
        d.send :__from_block__, 1

        no_splat.set!

        d.send_with_splat :other, 0, true, false
      end
    end
  end

  relates <<-ruby do
      def f
        begin
          b
        rescue
          c
        end

        d
      end
    ruby

    compile do |g|
      in_method :f do |d|

        d.for_rescue do |rb|
          rb.body do
            d.push :self
            d.send :b, 0, true
          end

          rb.condition :StandardError do
            d.push :self
            d.send :c, 0, true
          end
        end

        d.pop

        d.push :self
        d.send :d, 0, true
      end
    end
  end

  relates <<-ruby do
      def f
        a
        begin
          b
        rescue
          c
        end
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.push :self
        d.send :a, 0, true
        d.pop

        d.for_rescue do |rb|
          rb.body do
            d.push :self
            d.send :b, 0, true
          end

          rb.condition :StandardError do
            d.push :self
            d.send :c, 0, true
          end
        end

      end
    end
  end

  relates <<-ruby do
      def f
        a
        begin
          b
        rescue
          c
        end
        d
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.push :self
        d.send :a, 0, true
        d.pop

        d.for_rescue do |rb|
          rb.body do
            d.push :self
            d.send :b, 0, true
          end

          rb.condition :StandardError do
            d.push :self
            d.send :c, 0, true
          end
        end

        d.pop

        d.push :self
        d.send :d, 0, true
      end
    end
  end

  relates <<-ruby do
      def f(&block)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.block_arg 0
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(mand, opt = 42, &block)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.optional_arg 1
        d.block_arg 2
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(x, a=x.b)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        if_set = d.new_label
        d.passed_arg 1
        d.git if_set
        d.push_local 0
        d.send :b, 0, false
        d.set_local 1
        d.pop
        if_set.set!
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(mand, &block)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.block_arg 1
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(mand, opt = 42)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.optional_arg 1
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(mand, opt = 42, *rest, &block)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.optional_arg 1
        d.block_arg 3
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def x(a, b = 42, *)
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.optional_arg 1
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(mand, opt = 42, *rest)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.optional_arg 1
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def empty
      end
    ruby

    compile do |g|
      in_method :empty do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(mand)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(mand, *rest, &block)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.block_arg 2
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def x(a, *args)
        p(a, args)
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push :self
        d.push_local 0
        d.push_local 1
        d.send :p, 2, true
      end
    end
  end

  relates <<-ruby do
      def f(mand, *rest)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(opt = 42, &block)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.optional_arg 0
        d.block_arg 1
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(a = 42, b = '1', c=lambda {|n| n })
      end
    ruby

    # defn args opt lambda
  end

  relates <<-ruby do
      def f(opt = 42)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.optional_arg 0
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(opt = 42, *rest, &block)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.optional_arg 0
        d.block_arg 2
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def x(b = 42, *)
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.optional_arg 0
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(opt = 42, *rest)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.optional_arg 0
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def |(o)
      end
    ruby

    compile do |g|
      in_method :"|" do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def eql?(resource)
        (self.uuid == resource.uuid)
      rescue
        false
      end
    ruby

    compile do |g|
      in_method :eql? do |d|

        d.for_rescue do |rb|
          rb.body do
            d.push :self
            d.send :uuid, 0, false
            d.push_local 0
            d.send :uuid, 0, false
            d.send :==, 1, false
          end

          rb.condition :StandardError do
            d.push :false
          end
        end
      end
    end
  end

  relates <<-ruby do
      def something?
      end
    ruby

    compile do |g|
      in_method :something? do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def x(*)
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def f(*rest)
      end
    ruby

    compile do |g|
      in_method :f do |d|
        d.push :nil
      end
    end
  end

  relates <<-ruby do
      def x(a, *)
        p(a)
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push :self
        d.push_local 0
        d.send :p, 1, true
      end
    end
  end

  relates <<-ruby do
      def zarray
        a = []
        return a
      end
    ruby

    compile do |g|
      in_method :zarray do |d|
        d.make_array 0
        d.set_local 0
        d.pop
        d.push_local 0
        # TODO we emit a ret instruction even though the last statement
        # is itself a return, so we get to return instructions, one
        # after another. We could instead detect that an only output
        # the one.
        d.ret
      end
    end
  end

  relates <<-ruby do
      b = 42
      def a
        c do
          begin
            do_stuff
          rescue RuntimeError => b
            puts(b)
          end
        end
      end
    ruby

    compile do |g|
      g.push 42
      g.set_local 0
      g.pop

      g.in_method :a do |d|
        d.push :self

        d.in_block_send :c, 0 do |d2|

          d2.for_rescue do |rb|
            rb.body do
              d2.push :self
              d2.send :do_stuff, 0, true
            end

            rb.condition :RuntimeError do
              d2.push_current_exception
              d2.set_local 0
              d2.pop
              d2.push :self
              d2.push_local 0
              d2.send :puts, 1, true
            end
          end
        end
      end
    end
  end

  relates <<-ruby do
      def x(a=0.0,b=0.0)
        a+b
      end
    ruby

    compile do |g|
      in_method :x do |d|
        opt_arg_1 = d.new_label
        opt_arg_2 = d.new_label

        d.passed_arg 0
        d.git opt_arg_1
        d.push_literal 0.0
        d.set_local 0
        d.pop

        opt_arg_1.set!

        d.passed_arg 1
        d.git opt_arg_2
        d.push_literal 0.0
        d.set_local 1
        d.pop

        opt_arg_2.set!

        d.push_local 0
        d.push_local 1
        d.send :+, 1, false
      end
    end
  end

  relates <<-ruby do
      def x(*b)
        a(*b)
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push :self
        d.push_local 0
        d.cast_array
        d.push :nil
        d.send_with_splat :a, 0, true, false
      end
    end
  end

  relates <<-ruby do
      def meth(b)
        b
      end
    ruby

    compile do |g|
      in_method :meth do |d|
        d.push_local 0
      end
    end
  end
end
