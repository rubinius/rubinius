require File.dirname(__FILE__) + '/../spec_helper'

describe "A Defn node" do
  relates <<-ruby do
      def m
        begin
        end
      end
    ruby

    parse do
      [:defn, :m, [:args], [:scope, [:block, [:nil]]]]
    end

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
        d.push_literal :a
        d.ensure_return
        d.pop_unwind
        d.goto ensure_noexc_lbl

        ensure_exc_lbl.set!
        d.push_exception
        d.push_literal :b
        d.clear_exception
        d.ret
        d.pop
        d.pop_exception
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

    parse do
      [:defn,
       :blah,
       [:args, :"*args", :"&block"],
       [:scope,
        [:block,
         [:call,
          nil,
          :other,
          [:arglist,
           [:lit, 42],
           [:splat, [:lvar, :args]],
           [:block_pass, [:lvar, :block]]]]]]]
    end

    compile do |g|
      in_method :blah do |d|
        no_proc  = d.new_label
        no_splat = d.new_label

        d.push_block
        d.dup
        d.is_nil
        d.git no_proc

        d.push_cpath_top
        d.find_const :Proc
        d.swap
        d.send :__from_block__, 1

        no_proc.set!

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

    parse do
      [:defn,
       :blah,
       [:args, :"*args", :"&block"],
       [:scope,
        [:block,
         [:call,
          nil,
          :other,
          [:arglist,
           [:splat, [:lvar, :args]],
           [:block_pass, [:lvar, :block]]]]]]]
    end

    compile do |g|
      g.in_method :blah do |d|
        no_proc  = d.new_label
        no_splat = d.new_label

        d.push_block
        d.dup
        d.is_nil
        d.git no_proc

        d.push_cpath_top
        d.find_const :Proc
        d.swap
        d.send :__from_block__, 1

        no_proc.set!

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

    parse do
      [:defn,
       :f,
       [:args],
       [:scope,
        [:block,
         [:rescue,
          [:call, nil, :b, [:arglist]],
          [:resbody, [:array], [:call, nil, :c, [:arglist]]]],
         [:call, nil, :d, [:arglist]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.in_rescue :StandardError do |section|
          case section
          when :body then
            d.push :self
            d.send :b, 0, true
          when :StandardError then
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

    parse do
      [:defn,
       :f,
       [:args],
       [:scope,
        [:block,
         [:call, nil, :a, [:arglist]],
         [:rescue,
          [:call, nil, :b, [:arglist]],
          [:resbody, [:array], [:call, nil, :c, [:arglist]]]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push :self
        d.send :a, 0, true
        d.pop

        d.in_rescue :StandardError do |section|
          case section
          when :body then
            d.push :self
            d.send :b, 0, true
          when :StandardError
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

    parse do
      [:defn,
       :f,
       [:args],
       [:scope,
        [:block,
         [:call, nil, :a, [:arglist]],
         [:rescue,
          [:call, nil, :b, [:arglist]],
          [:resbody, [:array], [:call, nil, :c, [:arglist]]]],
         [:call, nil, :d, [:arglist]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push :self
        d.send :a, 0, true
        d.pop

        d.in_rescue :StandardError do |section|
          case section
          when :body then
            d.push :self
            d.send :b, 0, true
          when :StandardError then
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

    parse do
      [:defn, :f, [:args, :"&block"], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :f,
       [:args, :mand, :opt, :"&block", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :f,
       [:args, :x, :a, [:block, [:lasgn, :a, [:call, [:lvar, :x], :b, [:arglist]]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn, :f, [:args, :mand, :"&block"], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :f,
       [:args, :mand, :opt, [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :f,
       [:args,
        :mand,
        :opt,
        :"*rest",
        :"&block",
        [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :x,
       [:args, :a, :b, :*, [:block, [:lasgn, :b, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :f,
       [:args, :mand, :opt, :"*rest", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn, :empty, [:args], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn, :f, [:args, :mand], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn, :f, [:args, :mand, :"*rest", :"&block"], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :x,
       [:args, :a, :"*args"],
       [:scope, [:block, [:call, nil, :p, [:arglist, [:lvar, :a], [:lvar, :args]]]]]]
    end

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

    parse do
      [:defn, :f, [:args, :mand, :"*rest"], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :f,
       [:args, :opt, :"&block", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :f,
       [:args,
        :a,
        :b,
        :c,
        [:block,
         [:lasgn, :a, [:lit, 42]],
         [:lasgn, :b, [:str, "1"]],
         [:lasgn,
          :c,
          [:iter, [:call, nil, :lambda, [:arglist]], [:lasgn, :n], [:lvar, :n]]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args opt lambda
  end

  relates <<-ruby do
      def f(opt = 42)
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args, :opt, [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :f,
       [:args, :opt, :"*rest", :"&block", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :x,
       [:args, :b, :*, [:block, [:lasgn, :b, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :f,
       [:args, :opt, :"*rest", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn, :|, [:args, :o], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :eql?,
       [:args, :resource],
       [:scope,
        [:block,
         [:rescue,
          [:call,
           [:call, [:self], :uuid, [:arglist]],
           :==,
           [:arglist, [:call, [:lvar, :resource], :uuid, [:arglist]]]],
          [:resbody, [:array], [:false]]]]]]
    end

    compile do |g|
      in_method :eql? do |d|
        d.in_rescue :StandardError, 1 do |section|
          case section
          when :body then
            d.push :self
            d.send :uuid, 0, false
            d.push_local 0
            d.send :uuid, 0, false
            d.send :==, 1, false
          when :StandardError then
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

    parse do
      [:defn, :something?, [:args], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn, :x, [:args, :*], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn, :f, [:args, :"*rest"], [:scope, [:block, [:nil]]]]
    end

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

    parse do
      [:defn,
       :x,
       [:args, :a, :*],
       [:scope, [:block, [:call, nil, :p, [:arglist, [:lvar, :a]]]]]]
    end

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

    parse do
      [:defn,
       :zarray,
       [:args],
       [:scope, [:block, [:lasgn, :a, [:array]], [:return, [:lvar, :a]]]]]
    end

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

    parse do
      [:block,
       [:lasgn, :b, [:lit, 42]],
       [:defn,
        :a,
        [:args],
        [:scope,
         [:block,
          [:iter,
           [:call, nil, :c, [:arglist]],
           nil,
           [:rescue,
            [:call, nil, :do_stuff, [:arglist]],
            [:resbody,
             [:array, [:const, :RuntimeError], [:lasgn, :b, [:gvar, :$!]]],
             [:call, nil, :puts, [:arglist, [:lvar, :b]]]]]]]]]]
    end

    compile do |g|
      g.push 42
      g.set_local 0
      g.pop

      g.in_method :a do |d|
        d.push :self

        d.in_block_send :c, 0 do |d2|
          d2.in_rescue :RuntimeError do |section|
            case section
            when :body then
              d2.push :self
              d2.send :do_stuff, 0, true
            when :RuntimeError then
              d2.push_exception
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

    parse do
      [:defn,
       :x,
       [:args,
        :a,
        :b,
        [:block, [:lasgn, :a, [:lit, 0.0]], [:lasgn, :b, [:lit, 0.0]]]],
       [:scope, [:block, [:call, [:lvar, :a], :+, [:arglist, [:lvar, :b]]]]]]
    end

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

    parse do
      [:defn,
       :x,
       [:args, :"*b"],
       [:scope, [:block, [:call, nil, :a, [:arglist, [:splat, [:lvar, :b]]]]]]]
    end

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

    parse do
      [:defn, :meth, [:args, :b], [:scope, [:block, [:lvar, :b]]]]
    end

    compile do |g|
      in_method :meth do |d|
        d.push_local 0
      end
    end
  end
end
