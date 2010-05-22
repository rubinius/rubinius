require File.dirname(__FILE__) + '/../spec_helper'

describe "A Rescue node" do
  rescue_empty = lambda do |g|
    g.for_rescue do |rb|
      rb.body do
        g.push :self
        g.send :blah, 0, true
      end

      rb.condition :StandardError do
        g.push :nil
      end
    end
  end

  relates "blah rescue nil" do
    compile(&rescue_empty)
  end

  relates <<-ruby do
      begin
        blah
      rescue
      end
    ruby

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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push :self
          g.send :a, 0, true

        end

        rb.condition :A do
          g.push :self
          g.send :b, 0, true
        end

        rb.condition :B do
          g.push :self
          g.send :c, 0, true
        end

        rb.condition :C do
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push :self
          g.send :a, 0, true
        end

        rb.condition :StandardError do
          g.push_current_exception
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push :self
          g.send :a, 0, true
        end

        rb.condition :StandardError do
          g.push_current_exception
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push :self
          g.send :a, 0, true
        end

        rb.condition :StandardError do
          g.push_current_exception
          g.set_local 0
          g.pop
          g.push :nil
        end
      end

      g.pop

      g.for_rescue do |rb|
        rb.body do
          g.push :self
          g.send :b, 0, true
        end

        rb.condition :StandardError do
          g.push_current_exception
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push :self
          g.send :blah, 0, true
        end

        rb.condition :RuntimeError do
          g.push_current_exception
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 1
        end

        rb.condition :StandardError do
          g.push_current_exception
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 1
        end

        rb.condition :StandardError do
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 1
        end

        rb.condition :StandardError do
          g.push_current_exception
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 1
        end

        rb.condition :StandardError do
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 1
        end

        rb.condition :StandardError do
          g.push_current_exception
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 12
        end

        rb.condition :String do
          g.push 13
        end

        rb.els do
          g.pop
          g.push 14
        end
      end
    end
  end

  relates <<-ruby do
      begin
        12
      rescue *blah
        13
      end
    ruby

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 12
        end

        rb.raw_condition do |bl, nl|
          g.dup
          g.push :self
          g.send :blah, 0, true
          g.cast_array
          g.swap
          g.send :__rescue_match__, 1

          g.git bl
          g.goto nl

          bl.set!
          g.pop
          g.push 13
        end
      end
    end
  end

  relates <<-ruby do
      begin
        12
      rescue String, *blah
        13
      end
    ruby

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 12
        end

        rb.raw_condition do |body_label, next_label|
          g.dup
          g.push_const :String
          g.swap
          g.send :===, 1
          g.git body_label

          g.dup
          g.push :self
          g.send :blah, 0, true
          g.cast_array
          g.swap

          g.send :__rescue_match__, 1
          g.git body_label

          g.goto next_label

          body_label.set!
          g.pop

          g.push 13
        end
      end
    end
  end

  relates <<-ruby do
      begin
        12
      rescue *blah => e
        13
      end
    ruby

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 12
        end

        rb.raw_condition do |body_label,next_label|
          g.dup
          g.push :self
          g.send :blah, 0, true
          g.cast_array
          g.swap

          g.send :__rescue_match__, 1
          g.git body_label
          g.goto next_label

          body_label.set!

          g.pop
          g.push_current_exception
          g.set_local 0
          g.pop

          g.push 13
        end
      end
    end
  end

  relates <<-ruby do
      begin
        12
      rescue String, *blah => e
        13
      end
    ruby

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 12
        end

        rb.raw_condition do |body_label, next_label|
          g.dup
          g.push_const :String
          g.swap
          g.send :===, 1
          g.git body_label

          g.dup
          g.push :self
          g.send :blah, 0, true
          g.cast_array
          g.swap

          g.send :__rescue_match__, 1
          g.git body_label

          g.goto next_label

          body_label.set!
          g.pop

          g.push_current_exception
          g.set_local 0
          g.pop

          g.push 13
        end
      end
    end
  end

  relates <<-ruby do
      begin
        12
      rescue String
        return nil
      end
    ruby

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 12
        end

        rb.condition :String do
          g.push :nil
          rb.restore_exception
          g.ret
        end
      end
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

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 1
        end

        rb.condition :StandardError do
          g.for_rescue do |rb2|
            rb2.body do
              g.push 2
            end

            rb2.condition :StandardError do
              g.push 3
              g.push_stack_local 2
              g.restore_exception_state
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

    compile do |g|
      for_rescue do |rb|
        rb.body do
          g.push 1
        end

        rb.condition :StandardError do
          g.push_rubinius
          g.push_literal :x

          d = new_generator(g, :x)

          d.push 2
          d.ret
          d.ret

          g.push_literal(d)

          g.push_scope
          g.push_variables
          g.send :method_visibility, 0
          g.send :add_defn_method, 4
          g.pop

          g.push :self
          g.send :x, 0, true
        end
      end
    end
  end

  relates <<-ruby do
      begin
        while 1
          2
          break :brk
        end
      rescue
        3
      end
    ruby

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          top = g.new_label
          bottom = g.new_label
          brk = g.new_label

          g.push_modifiers

          top.set!
          g.push 1
          g.gif bottom

          # redo
          g.new_label.set!

          g.push 2
          g.pop
          g.push_literal :brk
          g.goto brk

          g.pop
          g.check_interrupts
          g.goto top

          bottom.set!
          g.push :nil

          brk.set!
          g.pop_modifiers
        end

        rb.condition :StandardError do
          g.push 3
        end
      end
    end
  end

  relates <<-ruby do
      begin
        1
      rescue
        while 2
          3
          break :brk
        end
      end
    ruby

    compile do |g|
      g.for_rescue do |rb|
        rb.body do
          g.push 1
        end

        rb.condition :StandardError do
          top = g.new_label
          bottom = g.new_label
          brk = g.new_label

          g.push_modifiers

          top.set!
          g.push 2
          g.gif bottom

          # redo
          g.new_label.set!

          g.push 3
          g.pop
          g.push_literal :brk
          rb.break

          g.pop
          g.check_interrupts
          g.goto top

          bottom.set!
          g.push :nil
          brk.set!

          g.pop_modifiers
        end
      end
    end
  end
end
