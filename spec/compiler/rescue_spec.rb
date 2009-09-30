require File.dirname(__FILE__) + '/../spec_helper'

describe "A Rescue node" do
  rescue_empty = lambda do |g|
    in_rescue :StandardError do |section|
      case section
      when :body then
        g.push :self
        g.send :blah, 0, true
      when :StandardError then
        g.push :nil
      end
    end
  end

  relates "blah rescue nil" do
    parse do
      [:rescue, [:call, nil, :blah, [:arglist]], [:resbody, [:array], [:nil]]]
    end

    compile(&rescue_empty)
  end

  relates <<-ruby do
      begin
        blah
      rescue
      end
    ruby

    parse do
      [:rescue, [:call, nil, :blah, [:arglist]], [:resbody, [:array], nil]]
    end

    compile(&rescue_empty)
  end

  relates <<-ruby do
      begin
        a
      rescue A
        b
      rescue B
        c
      rescue C
        d
      end
    ruby

    parse do
      [:rescue,
       [:call, nil, :a, [:arglist]],
       [:resbody, [:array, [:const, :A]], [:call, nil, :b, [:arglist]]],
       [:resbody, [:array, [:const, :B]], [:call, nil, :c, [:arglist]]],
       [:resbody, [:array, [:const, :C]], [:call, nil, :d, [:arglist]]]]
    end

    compile do |g|
      in_rescue :A, :B, :C do |section|
        case section
        when :body then
          g.push :self
          g.send :a, 0, true
        when :A then
          g.push :self
          g.send :b, 0, true
        when :B then
          g.push :self
          g.send :c, 0, true
        when :C then
          g.push :self
          g.send :d, 0, true
        end
      end
    end
  end

  relates <<-ruby do
      begin
        a
      rescue => @e
        c
        d
      end
    ruby

    parse do
      [:rescue,
       [:call, nil, :a, [:arglist]],
       [:resbody,
        [:array, [:iasgn, :@e, [:gvar, :$!]]],
        [:block, [:call, nil, :c, [:arglist]], [:call, nil, :d, [:arglist]]]]]
    end

    compile do |g|
      in_rescue :StandardError do |section|
        case section
        when :body then
          g.push :self
          g.send :a, 0, true
        when :StandardError then
          g.push_exception
          g.set_ivar :@e
          g.pop
          g.push :self
          g.send :c, 0, true
          g.pop
          g.push :self
          g.send :d, 0, true
        end
      end
    end
  end

  relates <<-ruby do
      begin
        a
      rescue => e
        c
        d
      end
    ruby

    parse do
      [:rescue,
       [:call, nil, :a, [:arglist]],
       [:resbody,
        [:array, [:lasgn, :e, [:gvar, :$!]]],
        [:block, [:call, nil, :c, [:arglist]], [:call, nil, :d, [:arglist]]]]]
    end

    compile do |g|
      in_rescue :StandardError, 1 do |section|
        case section
        when :body then
          g.push :self
          g.send :a, 0, true
        when :StandardError then
          g.push_exception
          g.set_local 0
          g.pop
          g.push :self
          g.send :c, 0, true
          g.pop
          g.push :self
          g.send :d, 0, true
        end
      end
    end
  end

  relates <<-ruby do
      begin
        a
      rescue => mes
        # do nothing
      end

      begin
        b
      rescue => mes
        # do nothing
      end
    ruby

    parse do
      [:block,
         [:rescue,
          [:call, nil, :a, [:arglist]],
          [:resbody, [:array, [:lasgn, :mes, [:gvar, :$!]]], nil]],
         [:rescue,
          [:call, nil, :b, [:arglist]],
          [:resbody, [:array, [:lasgn, :mes, [:gvar, :$!]]], nil]]]
    end

    compile do |g|
      in_rescue :StandardError, 1 do |section|
        case section
        when :body then
          g.push :self
          g.send :a, 0, true
        when :StandardError then
          g.push_exception
          g.set_local 0
          g.pop
          g.push :nil
        end
      end

      g.pop

      in_rescue :StandardError, 2 do |section|
        case section
        when :body then
          g.push :self
          g.send :b, 0, true
        when :StandardError then
          g.push_exception
          g.set_local 0
          g.pop
          g.push :nil
        end
      end
    end
  end

  relates <<-ruby do
      begin
        blah
      rescue RuntimeError => r
      end
    ruby

    parse do
      [:rescue,
       [:call, nil, :blah, [:arglist]],
       [:resbody,
        [:array, [:const, :RuntimeError], [:lasgn, :r, [:gvar, :$!]]],
        nil]]
    end

    compile do |g|
      in_rescue :RuntimeError, 1 do |section|
        case section
        when :body then
          g.push :self
          g.send :blah, 0, true
        when :RuntimeError then
          g.push_exception
          g.set_local 0
          g.pop
          g.push :nil
        end
      end
    end
  end

  relates <<-ruby do
      begin
        1
      rescue => @e
      end
    ruby

    parse do
      [:rescue, [:lit, 1], [:resbody, [:array, [:iasgn, :@e, [:gvar, :$!]]], nil]]
    end

    compile do |g|
      in_rescue :StandardError, 1 do |section|
        case section
        when :body then
          g.push 1
        when :StandardError then
          g.push_exception
          g.set_ivar :@e
          g.pop
          g.push :nil
        end
      end
    end
  end

  relates <<-ruby do
      begin
        1
      rescue
        var = 2
      end
    ruby

    parse do
      [:rescue, [:lit, 1], [:resbody, [:array], [:lasgn, :var, [:lit, 2]]]]
    end

    compile do |g|
      in_rescue :StandardError, 1 do |section|
        case section
        when :body then
          g.push 1
        when :StandardError then
          g.push 2
          g.set_local 0
        end
      end
    end
  end

  relates <<-ruby do
      begin
        1
      rescue => e
      end
    ruby

    parse do
      [:rescue, [:lit, 1], [:resbody, [:array, [:lasgn, :e, [:gvar, :$!]]], nil]]
    end

    compile do |g|
      in_rescue :StandardError, 1 do |section|
        case section
        when :body then
          g.push 1
        when :StandardError then
          g.push_exception
          g.set_local 0
          g.pop
          g.push :nil
        end
      end
    end
  end

  relates <<-ruby do
      begin
        1
      rescue
        a.b = nil
      end
    ruby

    parse do
      [:rescue,
       [:lit, 1],
       [:resbody,
        [:array],
        [:attrasgn, [:call, nil, :a, [:arglist]], :b=, [:arglist, [:nil]]]]]
    end

    compile do |g|
      in_rescue :StandardError, 1 do |section|
        case section
        when :body then
          g.push 1
        when :StandardError then
          g.push :self
          g.send :a, 0, true
          g.push :nil
          g.dup
          g.move_down 2
          g.send :b=, 1, false
          g.pop
        end
      end
    end
  end

  relates <<-ruby do
      begin
        1
      rescue => e
        var = 2
      end
    ruby

    parse do
      [:rescue,
       [:lit, 1],
       [:resbody, [:array, [:lasgn, :e, [:gvar, :$!]]], [:lasgn, :var, [:lit, 2]]]]
    end

    compile do |g|
      in_rescue :StandardError, 1 do |section|
        case section
        when :body then
          g.push 1
        when :StandardError then
          g.push_exception
          g.set_local 0
          g.pop
          g.push 2
          g.set_local 1
        end
      end
    end
  end

  relates <<-ruby do
      begin
        12
      rescue String
        13
      else
        14
      end
    ruby

    parse do
      [:rescue,
       [:lit, 12],
       [:resbody, [:array, [:const, :String]], [:lit, 13]],
       [:lit, 14]]
    end

    compile do |g|
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception

      rr.set!
      g.exceptions do |ex|
        g.push 12
        ex.escape fin

        ex.handle!
        g.push_const :String
        g.push_exception
        g.send :===, 1

        g.git body
        g.goto rr
        body.set!
        g.push 13
        g.clear_exception
        g.goto last

        rr.set!

        g.reraise

        fin.set!
        g.pop
        g.push 14

        last.set!
      end
      g.restore_exception
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      begin
        12
      rescue *blah
        13
      end
    ruby

    parse do
      [:rescue,
       [:lit, 12],
       [:resbody, [:splat, [:call, nil, :blah, [:arglist]]], [:lit, 13]]]
    end

    compile do |g|
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception

      rr.set!
      g.exceptions do |ex|
        g.push 12
        ex.escape fin

        ex.handle!
        g.push :self
        g.send :blah, 0, true
        g.cast_array
        g.push_exception
        g.send :__rescue_match__, 1

        g.git body
        g.goto rr
        body.set!
        g.push 13
        g.clear_exception
        g.goto last

        rr.set!

        g.reraise

        fin.set!

        last.set!
      end
      g.restore_exception
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      begin
        12
      rescue String, *blah
        13
      end
    ruby

    parse do
      [:rescue,
       [:lit, 12],
       [:resbody,
        [:array, [:const, :String], [:splat, [:call, nil, :blah, [:arglist]]]],
        [:lit, 13]]]
    end

    compile do |g|
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception

      rr.set!
      g.exceptions do |ex|
        g.push 12
        ex.escape fin

        ex.handle!

        g.push_const :String
        g.push_exception
        g.send :===, 1
        g.git body

        g.push :self
        g.send :blah, 0, true
        g.cast_array
        g.push_exception

        g.send :__rescue_match__, 1
        g.git body
        g.goto rr
        body.set!
        g.push 13
        g.clear_exception
        g.goto last

        rr.set!

        g.reraise

        fin.set!

        last.set!
      end
      g.restore_exception
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      begin
        12
      rescue *blah => e
        13
      end
    ruby

    parse do
      [:rescue,
       [:lit, 12],
       [:resbody,
        [:splat, [:call, nil, :blah, [:arglist]], [:lasgn, :e, [:gvar, :$!]]],
        [:lit, 13]]]
    end

    compile do |g|
      fin        = g.new_label
      rr         = g.new_label
      reraise    = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception

      rr.set!
      g.exceptions do |ex|
        g.push 12
        ex.escape fin

        ex.handle!

        g.push :self
        g.send :blah, 0, true
        g.cast_array
        g.push_exception
        g.set_local 0

        g.send :__rescue_match__, 1
        g.git body
        g.goto reraise
        body.set!

        g.push 13
        g.clear_exception
        g.goto last

        reraise.set!

        g.reraise

        fin.set!

        last.set!
      end
      g.restore_exception
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      begin
        12
      rescue String, *blah => e
        13
      end
    ruby

    parse do
      [:rescue,
       [:lit, 12],
       [:resbody,
        [:array,
         [:const, :String],
         [:splat, [:call, nil, :blah, [:arglist]]],
         [:lasgn, :e, [:gvar, :$!]]],
        [:lit, 13]]]
    end

    compile do |g|
      fin        = g.new_label
      rr         = g.new_label
      reraise    = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception

      rr.set!
      g.exceptions do |ex|
        g.push 12
        ex.escape fin

        ex.handle!

        g.push_const :String
        g.push_exception
        g.send :===, 1
        g.git body

        g.push :self
        g.send :blah, 0, true
        g.cast_array
        g.push_exception

        g.send :__rescue_match__, 1
        g.git body
        g.goto reraise
        body.set!
        g.push_exception
        g.set_local 0
        g.pop
        g.push 13
        g.clear_exception
        g.goto last

        reraise.set!

        g.reraise

        fin.set!

        last.set!
      end
      g.restore_exception
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      begin
        12
      rescue String
        return nil
      end
    ruby

    parse do
      [:rescue,
       [:lit, 12],
       [:resbody, [:array, [:const, :String]], [:return, [:nil]]]]
    end

    compile do |g|
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception

      rr.set!
      g.exceptions do |ex|
        g.push 12
        ex.escape fin

        ex.handle!
        g.push_const :String
        g.push_exception
        g.send :===, 1

        g.git body
        g.goto rr
        body.set!
        g.push :nil
        g.clear_exception
        g.ret
        g.clear_exception
        g.goto last

        rr.set!

        g.reraise

        fin.set!

        last.set!
      end
      g.restore_exception
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      begin
        1
      rescue
        begin
          2
        rescue
          return 3
        end
      end
    ruby

    parse do
      [:rescue,
       [:lit, 1],
       [:resbody,
        [:array],
        [:rescue, [:lit, 2], [:resbody, [:array], [:return, [:lit, 3]]]]]]
    end

    compile do |g|
      in_rescue :StandardError, 1 do |section|
        case section
        when :body
          g.push 1
        when :StandardError
          in_rescue :StandardError, 1 do |section|
            case section
            when :body
              g.push 2
            when :StandardError
              g.push 3
              g.clear_exception
              g.ret
            end
          end
        end
      end
    end
  end

  relates <<-ruby do
      begin
        1
      rescue
        def x
          return 2
        end
        x
      end
    ruby

    parse do
      [:rescue,
       [:lit, 1],
       [:resbody,
        [:array],
        [:block,
         [:defn, :x, [:args], [:scope, [:block, [:return, [:lit, 2]]]]],
         [:call, nil, :x, [:arglist]]]]]
    end

    compile do |g|
      in_rescue :StandardError, 1 do |section|
        case section
        when :body
          g.push 1
        when :StandardError
          g.push_const :Rubinius
          g.push_literal :x
          g.push_literal_desc :x do |d|
            d.push 2
            d.ret
            d.ret
          end
          g.push_scope
          g.push_variables
          g.send :method_visibility, 0
          g.send :add_defn_method, 4

          g.send :x, 0, true
        end
      end
    end
  end
end
