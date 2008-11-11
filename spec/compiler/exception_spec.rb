require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do

  it "compiles a rescue with an empty body and no else" do
    ruby = <<-EOC
      begin
      rescue O
        13
      end
    EOC

    sexp = s(:rescue,
             s(:resbody, s(:array, s(:const, :O)),
               s(:fixnum, 13)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers
      g.push :nil
      g.pop_modifiers
    end
  end

  it "compiles a rescue with an empty body with an else" do
    ruby = <<-EOC
      begin
      rescue O
        13
      else
        14
      end
    EOC

    sexp = s(:rescue,
             s(:resbody, s(:array, s(:const, :O)),
               s(:fixnum, 13)),
             s(:fixnum, 14))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers
      g.push 14
      g.pop_modifiers
    end

  end

  it "compiles a rescue with one condition" do
    ruby = <<-EOC
      begin
        12
      rescue String
        13
      end
    EOC

    sexp = s(:rescue, s(:fixnum, 12),
             s(:resbody, s(:array, s(:const, :String)),
               s(:fixnum, 13)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception
      exc_start.set!
      exc_start.set!
      g.push 12
      g.goto fin

      exc_handle.set!
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

      g.push_exception
      g.raise_exc

      fin.set!

      last.set!
      g.restore_exception
      g.pop_modifiers
    end
  end

  it "compiles a rescue with one condition into a local" do
    ruby = <<-EOC
      begin
        12
      rescue String => e
        13
      end
    EOC

    sexp = s(:rescue, s(:fixnum, 12),
             s(:resbody, s(:array, s(:const, :String),
                s(:lasgn, :e, s(:gvar, :$!))),
               s(:fixnum, 13)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception 1
      exc_start.set!
      exc_start.set!
      g.push 12
      g.goto fin

      exc_handle.set!
      g.push_const :String
      g.push_exception
      g.send :===, 1

      g.git body
      g.goto rr
      body.set!
      g.push_exception
      g.set_local 0
      g.push 13
      g.clear_exception
      g.goto last

      rr.set!

      g.push_exception
      g.raise_exc

      fin.set!

      last.set!
      g.restore_exception 1
      g.pop_modifiers
    end
  end

  it "compiles a rescue with one condition, no rescue class" do
    ruby = <<-EOC
      begin
        12
      rescue
        13
      end
    EOC

    sexp = s(:rescue,
             s(:fixnum, 12),
             s(:resbody, s(:array),
               s(:fixnum, 13)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception
      exc_start.set!
      exc_start.set!
      g.push 12
      g.goto fin

      exc_handle.set!

      g.push_const :StandardError
      g.push_exception
      g.send :===, 1
      g.git body
      g.goto rr
      body.set!

      g.push 13
      g.clear_exception
      g.goto last

      rr.set!

      g.push_exception
      g.raise_exc

      fin.set!

      last.set!
      g.restore_exception
      g.pop_modifiers
    end
  end


  it "compiles a rescue with multiple conditions" do
    ruby = <<-EOC
      begin
        12
      rescue String, Blah
        13
      end
    EOC

    sexp = s(:rescue,
             s(:fixnum, 12),
             s(:resbody,
               s(:array, s(:const, :String), s(:const, :Blah)),
               s(:fixnum, 13)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception
      exc_start.set!
      exc_start.set!
      g.push 12
      g.goto fin

      exc_handle.set!

      g.push_const :String
      g.push_exception
      g.send :===, 1
      g.git body

      g.push_const :Blah
      g.push_exception
      g.send :===, 1
      g.git body

      g.goto rr
      body.set!
      g.push 13
      g.clear_exception
      g.goto last

      rr.set!

      g.push_exception
      g.raise_exc

      fin.set!

      last.set!
      g.restore_exception
      g.pop_modifiers
    end
  end

  it "compiles multiple rescues" do
    ruby = <<-EOC
      begin
        12
      rescue String
        13
      rescue Blah
        14
      end
    EOC

    sexp = s(:rescue,
             s(:fixnum, 12),
             s(:resbody,
               s(:array, s(:const, :String)),
               s(:fixnum, 13)),
             s(:resbody,
               s(:array, s(:const, :Blah)),
               s(:fixnum, 14)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      next_cond  = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception
      exc_start.set!
      exc_start.set!
      g.push 12
      g.goto fin

      exc_handle.set!

      g.push_const :String
      g.push_exception
      g.send :===, 1
      g.git body

      g.goto next_cond
      body.set!
      g.push 13
      g.clear_exception
      g.goto last

      next_cond.set!
      body2 = g.new_label
      g.push_const :Blah
      g.push_exception
      g.send :===, 1
      g.git body2

      g.goto rr
      body2.set!
      g.push 14
      g.clear_exception
      g.goto last

      rr.set!

      g.push_exception
      g.raise_exc

      fin.set!

      last.set!
      g.restore_exception
      g.pop_modifiers
    end
  end

  it "compiles a rescue with a body and an else" do
    ruby = <<-EOC
      begin
        12
      rescue String
        13
      else
        14
      end
    EOC

    sexp = s(:rescue,
             s(:fixnum, 12),
             s(:resbody,
               s(:array, s(:const, :String)),
               s(:fixnum, 13)),
             s(:fixnum, 14))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception
      exc_start.set!
      exc_start.set!
      g.push 12
      g.goto fin

      exc_handle.set!
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

      g.push_exception
      g.raise_exc

      fin.set!
      g.pop
      g.push 14

      last.set!
      g.restore_exception
      g.pop_modifiers
    end
  end

  it "compiles a rescue with a splat" do
    ruby = <<-EOC
      begin
        12
      rescue *blah
        13
      end
    EOC

    sexp = s(:rescue,
             s(:fixnum, 12),
             s(:resbody,
               s(:splat, s(:call, nil, :blah, s(:arglist))),
               s(:fixnum, 13)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception
      exc_start.set!
      exc_start.set!
      g.push 12
      g.goto fin

      exc_handle.set!
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

      g.push_exception
      g.raise_exc

      fin.set!

      last.set!
      g.restore_exception
      g.pop_modifiers
    end

  end

  it "compiles a rescue with a condition and a splat" do
    ruby = <<-EOC
      begin
        12
      rescue String, *blah
        13
      end
    EOC

    sexp = s(:rescue,
             s(:fixnum, 12),
             s(:resbody,
               s(:array,
                 s(:const, :String),
                 s(:splat, s(:call, nil, :blah, s(:arglist)))),
               s(:fixnum, 13)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception
      exc_start.set!
      exc_start.set!
      g.push 12
      g.goto fin

      exc_handle.set!

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

      g.push_exception
      g.raise_exc

      fin.set!

      last.set!
      g.restore_exception
      g.pop_modifiers
    end

  end

  it "clears the exception when there is a return in a rescue" do
    ruby = <<-EOC
      begin
        12
      rescue String
        return nil
      end
    EOC

    sexp = s(:rescue, s(:fixnum, 12),
             s(:resbody,
               s(:array, s(:const, :String)),
               s(:return, s(:nil))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers
      g.save_exception
      exc_start.set!
      exc_start.set!
      g.push 12
      g.goto fin

      exc_handle.set!
      g.push_const :String
      g.push_exception
      g.send :===, 1

      g.git body
      g.goto rr
      body.set!
      g.clear_exception
      g.push :nil
      g.ret
      g.clear_exception
      g.goto last

      rr.set!

      g.push_exception
      g.raise_exc

      fin.set!

      last.set!
      g.restore_exception
      g.pop_modifiers
    end
  end

  it "compiles a basic ensure" do
    ruby = <<-EOC
      begin
        12
      ensure
        13
      end
    EOC

    sexp = s(:ensure, s(:fixnum, 12), s(:fixnum, 13))

    sexp.should == parse(ruby)

    gen sexp do |g|
      ok = g.new_label

      g.exceptions do |ex|
        g.push 12
        g.goto ok

        ex.handle!

        g.push 13
        g.pop

        g.push_exception
        g.raise_exc
      end

      ok.set!

      g.push 13
      g.pop
    end
  end

  # We don't have return in an ensure working yet.
=begin

  it "compiles an ensure with a return in the body" do
    ruby = <<-EOC
      NO
    EOC

    sexp = s(:ensure, s(:block, s(:fixnum, 14), s(:return, s(:fixnum, 2))), s(:fixnum, 13))

    sexp.should == parse(ruby) #

    gen sexp do |g|
      ok = g.new_label
      g.exceptions do |ex|
        g.push 14
        g.pop
        g.goto ok
        g.goto ok

        ex.handle do
          g.push 13
          g.pop

          g.push_exception
          g.raise_exc
        end
      end

      ok.set!
      g.push 13
      g.pop

      g.push 2
      g.ret

    end
  end

  it "compiles an ensure with a return in the body, inside another ensure" do
    ruby = <<-EOC
      NO
    EOC

    sexp = s(:ensure, s(:ensure, s(:block, s(:fixnum, 14), s(:return, s(:fixnum, 2))), s(:fixnum, 13)), s(:fixnum, 15))

    sexp.should == parse(ruby) #

    gen sexp do |g|
      ok = g.new_label
      ok2 = g.new_label

      g.exceptions do |ex|
        g.exceptions do |ex2|
          g.push 14
          g.pop
          g.goto ok2
          g.goto ok2

          ex2.handle do
            g.push 13
            g.pop

            g.push_exception
            g.raise_exc
          end

        end

        ok2.set!
        g.push 13
        g.pop
        g.goto ok
        g.goto ok

        ex.handle do
          g.push 15
          g.pop

          g.push_exception
          g.raise_exc
        end

      end

      ok.set!
      g.push 15
      g.pop

      g.push 2
      g.ret

    end
  end

  it "compiles an ensure with a return in the body, inside another ensure with a return" do
    ruby = <<-EOC
      NO
    EOC

    sexp = s(:ensure,
          s(:block, s(:fixnum, 14), s(:return, s(:fixnum, 2))),
          s(:ensure, s(:block, s(:fixnum, 15), s(:return, s(:fixnum, 3))),
             s(:fixnum, 16)))

    sexp.should == parse(ruby) #

    gen sexp do |g|
      ok = g.new_label
      ok2 = g.new_label

      g.exceptions do |ex|

        g.push 14
        g.pop
        g.goto ok
        g.goto ok

        ex.handle do

          g.exceptions do |ex2|
            g.push 15
            g.pop
            g.goto ok2
            g.goto ok2

            ex2.handle do
              g.push 16
              g.pop

              g.push_exception
              g.raise_exc
            end
          end

          g.pop

          g.push_exception
          g.raise_exc

          ok2.set!
          g.push 16
          g.pop

          g.push 3
          g.ret
        end

      end

      ok.set!

      ok3 = g.new_label
      g.exceptions do |ex2|
        g.push 15
        g.pop
        g.goto ok3
        g.goto ok3

        ex2.handle do
          g.push 16
          g.pop

          g.push_exception
          g.raise_exc
        end
      end

      ok3.set!
      g.push 16
      g.pop

      g.push 3
      g.ret
      g.pop

      g.push 2
      g.ret
    end
  end
=end

end
