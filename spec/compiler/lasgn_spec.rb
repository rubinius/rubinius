require File.dirname(__FILE__) + '/../spec_helper'

describe "An Lasgn node" do
  relates "a = 1" do
    parse do
      [:lasgn, :a, [:lit, 1]]
    end

    compile do |g|
      g.push 1
      g.set_local 0
    end
  end

  relates "a = b, c, *d" do
    parse do
      [:lasgn,
         :a,
         [:svalue,
          [:array,
           [:call, nil, :b, [:arglist]],
           [:call, nil, :c, [:arglist]],
           [:splat, [:call, nil, :d, [:arglist]]]]]]
    end

    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.push :self
      g.send :c, 0, true
      g.make_array 2
      g.push :self
      g.send :d, 0, true
      g.cast_array
      g.send :+, 1
      g.set_local 0
    end
  end

  relates "a = [b, *c]" do
    parse do
      [:lasgn,
         :a,
         [:array,
          [:call, nil, :b, [:arglist]],
          [:splat, [:call, nil, :c, [:arglist]]]]]
    end

    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.make_array 1
      g.push :self
      g.send :c, 0, true
      g.cast_array
      g.send :+, 1
      g.set_local 0
    end
  end

  relates <<-ruby do
      x = (y = 1
           (y + 2))
    ruby

    parse do
      [:lasgn,
         :x,
         [:block,
          [:lasgn, :y, [:lit, 1]],
          [:call, [:lvar, :y], :+, [:arglist, [:lit, 2]]]]]
    end

    compile do |g|
      g.push 1
      g.set_local 1
      g.pop
      g.push_local 1
      g.push 2
      g.send :+, 1, false
      g.set_local 0
    end
  end

  relates <<-ruby do
      a = []
      a [42]
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:array]],
       [:call, [:lvar, :a], :[], [:arglist, [:lit, 42]]]]
    end

    # call index space
  end

  relates 'var = ["foo", "bar"]' do
    parse do
      [:lasgn, :var, [:array, [:str, "foo"], [:str, "bar"]]]
    end

    compile do |g|
      g.push_literal "foo"
      g.string_dup
      g.push_literal "bar"
      g.string_dup
      g.make_array 2
      g.set_local 0
    end
  end

  relates "c = (2 + 3)" do
    parse do
      [:lasgn, :c, [:call, [:lit, 2], :+, [:arglist, [:lit, 3]]]]
    end

    compile do |g|
      g.push 2
      g.push 3
      g.send :+, 1, false
      g.set_local 0
    end
  end

  relates "a = *[1]" do
    parse do
      [:lasgn, :a, [:svalue, [:splat, [:array, [:lit, 1]]]]]
    end

    compile do |g|
      g.splatted_array
      g.set_local 0
    end
  end

  relates "a = *b" do
    parse do
      [:lasgn, :a, [:svalue, [:splat, [:call, nil, :b, [:arglist]]]]]
    end

    compile do |g|
      t = g.new_label

      g.push :self
      g.send :b, 0, true
      g.cast_array
      g.dup

      g.send :size, 0
      g.push 1
      g.send :>, 1
      g.git t

      g.push 0
      g.send :at, 1
      t.set!
      g.set_local 0
    end
  end

  relates <<-ruby do
      a = if c
            begin
              b
            rescue
              nil
            end
          end
      a
    ruby

    parse do
      [:block,
       [:lasgn,
        :a,
        [:if,
         [:call, nil, :c, [:arglist]],
         [:rescue, [:call, nil, :b, [:arglist]], [:resbody, [:array], [:nil]]],
         nil]],
       [:lvar, :a]]
    end

    compile do |g|
      bottom = g.new_label
      f      = g.new_label

      g.push :self
      g.send :c, 0, true
      g.gif f

      in_rescue :StandardError, 1 do |section|
        case section
        when :body then
          g.push :self
          g.send :b, 0, true
        when :StandardError then
          g.push :nil
        end
      end
      g.goto bottom

      f.set!
      g.push :nil

      bottom.set!
      g.set_local 0
      g.pop
      g.push_local 0
    end
  end

  relates "x = [*[1]]" do
    parse do
      [:lasgn, :x, [:array, [:splat, [:array, [:lit, 1]]]]]
    end

    compile do |g|
      g.push 1
      g.make_array 1
      g.set_local 0
    end
  end

  relates "a = []" do
    parse do
      [:lasgn, :a, [:array]]
    end

    compile do |g|
      g.make_array 0
      g.set_local 0
    end
  end

  relates <<-ruby do
      a = 12
      a
    ruby

    parse do
      [:block, [:lasgn, :a, [:lit, 12]], [:lvar, :a]]
    end

    compile do |g|
      g.push 12
      g.set_local 0
      g.pop
      g.push_local 0
    end
  end

  relates <<-ruby do
      name
      name = 3
      name
    ruby

    parse do
      [:block,
       [:call, nil, :name, [:arglist]],
       [:lasgn, :name, [:lit, 3]],
       [:lvar, :name]]
    end
  end

  relates "a=12; b=13; true" do
    parse do
      [:block, [:lasgn, :a, [:lit, 12]], [:lasgn, :b, [:lit, 13]], [:true]]
    end

    compile do |g|
      g.push 12
      g.set_local 0
      g.pop
      g.push 13
      g.set_local 1
      g.pop
      g.push :true
    end
  end

  relates <<-ruby do
      def f
        a = 1
      end
    ruby

    parse do
      [:defn, :f, [:args], [:scope, [:block, [:lasgn, :a, [:lit, 1]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push 1
        d.set_local 0
      end
    end
  end

  relates <<-ruby do
      def f(a)
        a = 1
      end
    ruby

    parse do
      [:defn, :f, [:args, :a], [:scope, [:block, [:lasgn, :a, [:lit, 1]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push 1
        d.set_local 0
      end
    end
  end

  relates <<-ruby do
      def f
        b { a = 1 }
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args],
       [:scope,
        [:block,
         [:iter, [:call, nil, :b, [:arglist]], nil, [:lasgn, :a, [:lit, 1]]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push :self

        d.in_block_send :b, :none do |e|
          e.push 1
          e.set_local 0
        end
      end
    end
  end

  relates <<-ruby do
      def f(a)
        b { a = 2 }
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args, :a],
       [:scope,
        [:block,
         [:iter, [:call, nil, :b, [:arglist]], nil, [:lasgn, :a, [:lit, 2]]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push :self

        d.in_block_send :b, :none do |e|
          e.push 2
          e.set_local_depth 1, 0
        end
      end
    end
  end

  relates <<-ruby do
      def f
        a = 1
        b { a = 2 }
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args],
       [:scope,
        [:block,
         [:lasgn, :a, [:lit, 1]],
         [:iter, [:call, nil, :b, [:arglist]], nil, [:lasgn, :a, [:lit, 2]]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push 1
        d.set_local 0
        d.pop

        d.push :self

        d.in_block_send :b, :none do |e|
          e.push 2
          e.set_local_depth 1, 0
        end
      end
    end
  end

  relates <<-ruby do
      def f
        a
        b { a = 1 }
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args],
       [:scope,
        [:block,
         [:call, nil, :a, [:arglist]],
         [:iter, [:call, nil, :b, [:arglist]], nil, [:lasgn, :a, [:lit, 1]]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push :self
        d.send :a, 0, true
        d.pop

        d.push :self

        d.in_block_send :b, :none do |e|
          e.push 1
          e.set_local 0
        end
      end
    end
  end

  relates <<-ruby do
      def f
        b { a = 1 }
        a
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args],
       [:scope,
        [:block,
         [:iter, [:call, nil, :b, [:arglist]], nil, [:lasgn, :a, [:lit, 1]]],
         [:call, nil, :a, [:arglist]]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push :self

        d.in_block_send :b, :none do |e|
          e.push 1
          e.set_local 0
        end

        d.pop
        d.push :self
        d.send :a, 0, true
      end
    end
  end

  relates <<-ruby do
      def f
        a = 1
        b { a = 2 }
        a
      end
    ruby

    parse do
      [:defn,
       :f,
       [:args],
       [:scope,
        [:block,
         [:lasgn, :a, [:lit, 1]],
         [:iter, [:call, nil, :b, [:arglist]], nil, [:lasgn, :a, [:lit, 2]]],
         [:lvar, :a]]]]
    end

    compile do |g|
      in_method :f do |d|
        d.push 1
        d.set_local 0
        d.pop

        d.push :self

        d.in_block_send :b, :none do |e|
          e.push 2
          e.set_local_depth 1, 0
        end

        d.pop
        d.push_local 0
      end
    end
  end

  relates <<-ruby do
      class F
        a = 1
      end
    ruby

    parse do
      [:class, :F, nil, [:scope, [:lasgn, :a, [:lit, 1]]]]
    end

    compile do |g|
      in_class :F do |d|
        d.push 1
        d.set_local 0
      end
    end
  end

  relates <<-ruby do
      a = Object
      class a::F
        b = 1
      end
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:const, :Object]],
       [:class, [:colon2, [:lvar, :a], :F], nil, [:scope, [:lasgn, :b, [:lit, 1]]]]]
    end

    compile do |g|
      g.push_const :Object
      g.set_local 0
      g.pop

      g.push_const :Rubinius
      g.push_literal :F
      g.push :nil
      g.push_local 0
      g.send :open_class_under, 3
      g.dup
      g.push_const :Rubinius
      g.swap
      g.push_literal :__class_init__
      g.swap

      g.push_literal_desc :F do |d|
        d.push_self
        d.add_scope

        d.push 1
        d.set_local 0

        d.ret
      end

      g.swap
      g.push_scope
      g.swap
      g.send :attach_method, 4
      g.pop
      g.send :__class_init__, 0
    end
  end

  relates <<-ruby do
      a = Object
      class F < a
        b = 1
      end
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:const, :Object]],
       [:class, :F, [:lvar, :a], [:scope, [:lasgn, :b, [:lit, 1]]]]]
    end

    compile do |g|
      g.push_const :Object
      g.set_local 0
      g.pop

      g.push_const :Rubinius
      g.push_literal :F
      g.push_local 0
      g.push_scope
      g.send :open_class, 3

      g.dup
      g.push_const :Rubinius
      g.swap
      g.push_literal :__class_init__
      g.swap

      g.push_literal_desc :F do |d|
        d.push_self
        d.add_scope

        d.push 1
        d.set_local 0

        d.ret
      end

      g.swap
      g.push_scope
      g.swap
      g.send :attach_method, 4
      g.pop
      g.send :__class_init__, 0
    end
  end

  relates <<-ruby do
      class F
        a = 1
        def f
          a = 1
        end
      end
    ruby

    parse do
      [:class,
       :F,
       nil,
       [:scope,
        [:block,
         [:lasgn, :a, [:lit, 1]],
         [:defn, :f, [:args], [:scope, [:block, [:lasgn, :a, [:lit, 1]]]]]]]]
    end

    compile do |g|
      in_class :F do |d|
        d.push 1
        d.set_local 0
        d.pop

        d.in_method :f do |e|
          e.push 1
          e.set_local 0
        end
      end
    end
  end

  relates <<-ruby do
      module M
        a = 1
      end
    ruby

    parse do
      [:module, :M, [:scope, [:lasgn, :a, [:lit, 1]]]]
    end

    compile do |g|
      in_module :M do |d|
        d.push 1
        d.set_local 0
      end
    end
  end

  relates <<-ruby do
      module M
        a = 1
        def f
          a = 1
        end
      end
    ruby

    parse do
      [:module,
       :M,
       [:scope,
        [:block,
         [:lasgn, :a, [:lit, 1]],
         [:defn, :f, [:args], [:scope, [:block, [:lasgn, :a, [:lit, 1]]]]]]]]
    end

    compile do |g|
      in_module :M do |d|
        d.push 1
        d.set_local 0
        d.pop

        d.in_method :f do |e|
          e.push 1
          e.set_local 0
        end
      end
    end
  end

  relates <<-ruby do
      a = Object
      module a::M
        b = 1
      end
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:const, :Object]],
       [:module, [:colon2, [:lvar, :a], :M], [:scope, [:lasgn, :b, [:lit, 1]]]]]
    end

    compile do |g|
      g.push_const :Object
      g.set_local 0
      g.pop

      g.push_const :Rubinius
      g.push_literal :M
      g.push_local 0
      g.send :open_module_under, 2
      g.dup
      g.push_const :Rubinius
      g.swap
      g.push_literal :__module_init__
      g.swap

      g.push_literal_desc :F do |d|
        d.push_self
        d.add_scope

        d.push 1
        d.set_local 0

        d.ret
      end

      g.swap
      g.push_scope
      g.swap
      g.send :attach_method, 4
      g.pop
      g.send :__module_init__, 0
    end
  end
end
