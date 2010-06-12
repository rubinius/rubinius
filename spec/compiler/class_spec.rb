require File.expand_path('../../spec_helper', __FILE__)

describe "A Class node" do
  relates <<-ruby do
      class X
        puts((1 + 1))
        def blah
          puts("hello")
        end
      end
    ruby

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

    compile do |g|
      g.push_rubinius
      g.push_literal :Y
      g.push :nil
      g.push_cpath_top
      g.send :open_class_under, 3
      g.dup
      g.push_rubinius
      g.swap
      g.push_literal :__class_init__
      g.swap

      d = new_generator(g, :Y)

      d.push_self
      d.add_scope
      d.push :self
      d.send :c, 0, true
      d.ret

      g.push_literal(d)

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

    compile do |g|
      g.push_rubinius
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

    compile do |g|
      g.push_rubinius
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

    compile do |g|
      g.push_rubinius
      g.push_literal :X
      g.push_const :Object
      g.push_scope
      g.send :open_class, 3
      g.pop
      g.push :nil
    end
  end
end
