require File.dirname(__FILE__) + '/../spec_helper'

describe "A Class node" do
  relates <<-ruby do
      class X
        puts((1 + 1))
        def blah
          puts("hello")
        end
      end
    ruby

    parse do
      [:class,
       :X,
       nil,
       [:scope,
        [:block,
         [:call,
          nil,
          :puts,
          [:arglist, [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]]],
         [:defn,
          :blah,
          [:args],
          [:scope,
           [:block, [:call, nil, :puts, [:arglist, [:str, "hello"]]]]]]]]]
    end

    compile do |g|
      in_class :X do |d|
        d.push :self
        d.push 1
        d.push 1
        d.send :+, 1, false
        d.send :puts, 1, true
        d.pop

        d.in_method :blah do |d2|
          d2.push :self
          d2.push_literal "hello"
          d2.string_dup
          d2.send :puts, 1, true
        end
      end
    end
  end

  relates <<-ruby do
      class ::Y
        c
      end
    ruby

    parse do
      [:class, [:colon3, :Y], nil, [:scope, [:call, nil, :c, [:arglist]]]]
    end

    compile do |g|
      g.push_const :Rubinius
      g.push_literal :Y
      g.push :nil
      g.push_cpath_top
      g.send :open_class_under, 3
      g.dup
      g.push_const :Rubinius
      g.swap
      g.push_literal :__class_init__
      g.swap
      g.push_literal_desc :Y do |d|
        d.push_self
        d.add_scope
        d.push :self
        d.send :c, 0, true
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
      class X::Y
        c
      end
    ruby

    parse do
      [:class,
       [:colon2, [:const, :X], :Y],
       nil,
       [:scope, [:call, nil, :c, [:arglist]]]]
    end

    compile do |g|
      in_class "X::Y" do |d|
        d.push :self
        d.send :c, 0, true
      end
    end
  end

  relates <<-ruby do
      class X < Array
      end
    ruby

    parse do
      [:class, :X, [:const, :Array], [:scope]]
    end

    compile do |g|
      g.push_const :Rubinius
      g.push_literal :X
      g.push_const :Array
      g.push_scope
      g.send :open_class, 3
      g.pop
      g.push :nil
    end
  end

  relates <<-ruby do
      class X < expr
      end
    ruby

    parse do
      [:class, :X, [:call, nil, :expr, [:arglist]], [:scope]]
    end

    compile do |g|
      g.push_const :Rubinius
      g.push_literal :X
      g.push :self
      g.send :expr, 0, true
      g.push_scope
      g.send :open_class, 3

      g.pop
      g.push :nil
    end
  end

  relates <<-ruby do
      class X < Object
      end
    ruby

    parse do
      [:class, :X, [:const, :Object], [:scope]]
    end

    compile do |g|
      g.push_const :Rubinius
      g.push_literal :X
      g.push_const :Object
      g.push_scope
      g.send :open_class, 3
      g.pop
      g.push :nil
    end
  end
end
