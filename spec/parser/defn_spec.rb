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

    # begin_def
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

    # block_pass_args_and_splat
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

    # block pass splat
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

    # block stmt after
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

    # block stmt before
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

    # block stmt both
  end

  relates <<-ruby do
      def f(&block)
        # do nothing
      end
    ruby

    parse do
      [:defn, :f, [:args, :"&block"], [:scope, [:block, [:nil]]]]
    end

    # defn args block
  end

  relates <<-ruby do
      def f(mand, opt = 42, &block)
        # do nothing
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args, :mand, :opt, :"&block", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args mand opt block
  end

  relates <<-ruby do
      def f(mand, &block)
        # do nothing
      end
    ruby

    parse do
      [:defn, :f, [:args, :mand, :"&block"], [:scope, [:block, [:nil]]]]
    end

    # defn args mand block
  end

  relates <<-ruby do
      def f(mand, opt = 42)
        # do nothing
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args, :mand, :opt, [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args mand opt
  end

  relates <<-ruby do
      def f(mand, opt = 42, *rest, &block)
        # do nothing
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

    # defn args mand opt splat block
  end

  relates <<-ruby do
      def x(a, b = 42, *)
        # do nothing
      end
    ruby

    parse do
      [:defn,
       :x,
       [:args, :a, :b, :*, [:block, [:lasgn, :b, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args mand opt splat no name
  end

  relates <<-ruby do
      def f(mand, opt = 42, *rest)
        # do nothing
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args, :mand, :opt, :"*rest", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args mand opt splat
  end

  relates <<-ruby do
      def empty
        # do nothing
      end
    ruby

    parse do
      [:defn, :empty, [:args], [:scope, [:block, [:nil]]]]
    end

    # defn args none
  end

  relates <<-ruby do
      def f(mand)
        # do nothing
      end
    ruby

    parse do
      [:defn, :f, [:args, :mand], [:scope, [:block, [:nil]]]]
    end

    # defn args mand
  end

  relates <<-ruby do
      def f(mand, *rest, &block)
        # do nothing
      end
    ruby

    parse do
      [:defn, :f, [:args, :mand, :"*rest", :"&block"], [:scope, [:block, [:nil]]]]
    end

    # defn args mand splat block
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

    # defn args mand splat no name
  end

  relates <<-ruby do
      def f(mand, *rest)
        # do nothing
      end
    ruby

    parse do
      [:defn, :f, [:args, :mand, :"*rest"], [:scope, [:block, [:nil]]]]
    end

    # defn args mand splat
  end

  relates <<-ruby do
      def f(opt = 42, &block)
        # do nothing
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args, :opt, :"&block", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args opt block
  end

  relates <<-ruby do
      def f(a = 42, b = '1', c=lambda {|n| n })
        # do nothing
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
        # do nothing
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args, :opt, [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args opt
  end

  relates <<-ruby do
      def f(opt = 42, *rest, &block)
        # do nothing
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args, :opt, :"*rest", :"&block", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args opt splat block
  end

  relates <<-ruby do
      def x(b = 42, *)
        # do nothing
      end
    ruby

    parse do
      [:defn,
       :x,
       [:args, :b, :*, [:block, [:lasgn, :b, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args opt splat no name
  end

  relates <<-ruby do
      def f(opt = 42, *rest)
        # do nothing
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args, :opt, :"*rest", [:block, [:lasgn, :opt, [:lit, 42]]]],
       [:scope, [:block, [:nil]]]]
    end

    # defn args opt splat
  end

  relates <<-ruby do
      def |(o)
        # do nothing
      end
    ruby

    parse do
      [:defn, :|, [:args, :o], [:scope, [:block, [:nil]]]]
    end

    # defn or
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

    # defn rescue mri verbose flag
  end

  relates <<-ruby do
      def something?
        # do nothing
      end
    ruby

    parse do
      [:defn, :something?, [:args], [:scope, [:block, [:nil]]]]
    end

    # defn something eh
  end

  relates <<-ruby do
      def x(*)
        # do nothing
      end
    ruby

    parse do
      [:defn, :x, [:args, :*], [:scope, [:block, [:nil]]]]
    end

    # defn args splat no name
  end

  relates <<-ruby do
      def f(*rest)
        # do nothing
      end
    ruby

    parse do
      [:defn, :f, [:args, :"*rest"], [:scope, [:block, [:nil]]]]
    end

    # defn args splat
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

    # defn splat no name
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

    # defn zarray
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

    # lvar def boundary
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

    # parse floats as args
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

    # splat
  end
end
