require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles if/end" do
    ruby = <<-EOC
      10 if true
    EOC

    sexp = s(:if, s(:true), s(:fixnum, 10), nil)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push :true
      els = g.new_label
      fin = g.new_label
      g.gif els
      g.push 10
      g.goto fin
      els.set!
      g.push :nil
      fin.set!
    end
  end

  it "compiles if/else/end" do
    ruby = <<-EOC
      if true then
        10
      else
        12
      end
    EOC

    sexp = s(:if, s(:true), s(:fixnum, 10), s(:fixnum, 12))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push :true
      els = g.new_label
      fin = g.new_label
      g.gif els
      g.push 10
      g.goto fin
      els.set!
      g.push 12
      fin.set!
    end
  end

  it "compiles unless/end" do
    ruby = <<-EOC
      unless true then
        12
      end
    EOC

    sexp = s(:if, s(:true), nil, s(:fixnum, 12))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push :true
      els = g.new_label
      fin = g.new_label
      g.git els
      g.push 12
      g.goto fin
      els.set!
      g.push :nil
      fin.set!
    end
  end

  it "compiles a stupid if" do
    ruby = <<-EOC
      if true then
      end
    EOC

    sexp = s(:if, s(:true), nil, nil)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push :true
      g.pop
      g.push :nil
    end
  end

  it "compiles a normal while" do
    ruby = <<-EOC
      while true do
        12
      end
    EOC

    sexp = s(:while, s(:true), s(:fixnum, 12), true)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers

      top = g.new_label
      fin = g.new_label
      g.redo = g.new_label


      top.set!
      g.push :true
      g.gif fin
      g.redo.set!
      g.push 12
      g.pop
      g.goto top

      fin.set!
      g.push :nil

      # Break
      g.new_label.set!

      g.pop_modifiers
    end
  end

  it "compiles a post while" do
    ruby = <<-EOC
      begin 12 end while true
    EOC

    sexp = s(:while, s(:true), s(:fixnum, 12), false)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers

      top = g.new_label
      fin = g.new_label

      top.set!
      g.push 12
      g.pop
      g.new_label.set! # next
      g.push :true
      g.gif fin
      g.goto top

      fin.set!
      g.push :nil

      # Break
      g.new_label.set!

      g.pop_modifiers
    end
  end

  it "compiles a normal until" do
    ruby = <<-EOC
      until true
        12
      end
    EOC

    sexp = s(:until, s(:true), s(:fixnum, 12), true)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers

      top = g.new_label
      fin = g.new_label
      g.redo = g.new_label

      top.set!
      g.push :true
      g.git fin
      g.redo.set!
      g.push 12
      g.pop
      g.goto top

      fin.set!
      g.push :nil

      # Break
      g.new_label.set!

      g.pop_modifiers
    end
  end

  it "compiles a post until" do
    ruby = <<-EOC
      begin 12 end until true
    EOC

    sexp = s(:until, s(:true), s(:fixnum, 12), false)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers

      top = g.new_label
      fin = g.new_label

      top.set!
      g.push 12
      g.pop
      g.new_label.set! # next
      g.push :true
      g.git fin
      g.goto top

      fin.set!
      g.push :nil

      # Break
      g.new_label.set!

      g.pop_modifiers
    end
  end

  it "compiles an each call" do
    ruby = <<-EOC
      (1..2).each do |x| end
    EOC

    sexp = s(:iter,
             s(:call, s(:dot2, s(:fixnum, 1), s(:fixnum, 2)), :each,
               s(:arglist)),
             s(:lasgn, :x))

    sexp.should == parse(ruby)

    gen(sexp) do |g|
      iter = description do |d|
        d.cast_for_single_block_arg
        d.set_local_depth 0, 0
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.ret
      end

      g.push_cpath_top
      g.find_const :Range
      g.push 1
      g.push 2
      g.send :new, 2
      g.create_block iter
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles an each call with multiple block arguments" do
    ruby = <<-EOC
      x.each do |a, b|
        5
      end
    EOC

    sexp = s(:iter,
             s(:call, s(:call, nil, :x, s(:arglist)), :each, s(:arglist)),
             s(:masgn, s(:array, s(:lasgn, :a), s(:lasgn, :b))),
             s(:fixnum, 5))

    sexp.should == parse(ruby)

    gen(sexp) do |g|
      iter = description do |d|
        d.cast_for_multi_block_arg
        d.shift_tuple
        d.set_local_depth 0, 0
        d.pop
        d.shift_tuple
        d.set_local_depth 0, 1
        d.pop
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push 5
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.send :x, 0, true
      g.create_block iter
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles a for loop" do
    ruby = <<-EOC
      for x in 1..2 do end
    EOC

    sexp = s(:for,
             s(:dot2, s(:fixnum, 1), s(:fixnum, 2)),
             s(:lasgn, :x))

    sexp.should == parse(ruby)

    gen(sexp) do |g|
      iter = description do |d|
        d.cast_for_single_block_arg
        d.set_local 0
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.ret
      end

      g.push_cpath_top
      g.find_const :Range
      g.push 1
      g.push 2
      g.send :new, 2
      g.create_block iter
      g.passed_block(1) do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles a for loop with an ivar assignment" do
    ruby = <<-EOC
      for @xyzzy in (1..2) do end
    EOC

    sexp = s(:for,
             s(:dot2, s(:fixnum, 1), s(:fixnum, 2)),
             s(:iasgn, :@xyzzy))

    sexp.should == parse(ruby)

    gen(sexp) do |g|
      iter = description do |d|
        d.cast_for_single_block_arg
        d.set_ivar :@xyzzy
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.ret
      end

      g.push_cpath_top
      g.find_const :Range
      g.push 1
      g.push 2
      g.send :new, 2
      g.create_block iter
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles a for loop with multiple arguments" do
    ruby = <<-EOC
      for a, b in x do
        5
      end
    EOC

    sexp = s(:for,
             s(:call, nil, :x, s(:arglist)),
             s(:masgn, s(:array, s(:lasgn, :a), s(:lasgn, :b))),
             s(:fixnum, 5))

    sexp.should == parse(ruby)

    gen(sexp) do |g|
      iter = description do |d|
        d.cast_for_multi_block_arg
        d.shift_tuple
        d.set_local 0
        d.pop
        d.shift_tuple
        d.set_local 1
        d.pop
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push 5
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.send :x, 0, true
      g.create_block iter
      g.passed_block(2) do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles a for loop with multiple arguments and an inner lasgn" do
    ruby = <<-EOC
      for a, b in x do
        z = 5
      end
    EOC

    sexp = s(:for,
             s(:call, nil, :x, s(:arglist)),
             s(:masgn, s(:array, s(:lasgn, :a), s(:lasgn, :b))),
             s(:lasgn, :z, s(:fixnum, 5)))

    sexp.should == parse(ruby)

    gen(sexp) do |g|
      iter = description do |d|
        d.cast_for_multi_block_arg
        d.shift_tuple
        d.set_local 0
        d.pop
        d.shift_tuple
        d.set_local 1
        d.pop
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push 5
        d.set_local_depth 0,0
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.send :x, 0, true
      g.create_block iter
      g.passed_block(2) do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles a series of expressions" do
    ruby = <<-EOC
      a=12; b=13; true
    EOC

    sexp = s(:block,
             s(:lasgn, :a, s(:fixnum, 12)),
             s(:lasgn, :b, s(:fixnum, 13)),
             s(:true))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push 12
      g.set_local 0
      g.pop
      g.push 13
      g.set_local 1
      g.pop
      g.push :true
    end
  end

  it "compiles loop directly" do
    ruby = <<-EOC
      loop { 12 }
    EOC

    sexp = s(:iter,
             s(:call, nil, :loop, s(:arglist)),
             nil,
             s(:fixnum, 12))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers

      top = g.new_label
      top.set!
      g.push 12
      g.pop
      g.goto top
      g.new_label.set! # break

      g.pop_modifiers
    end
  end

  it "compiles empty loop directly" do
    ruby = <<-EOC
      loop {}
    EOC

    sexp = s(:iter, s(:call, nil, :loop, s(:arglist)), nil)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers
      top = g.new_label
      top.set!
      # No body, no stack manipulation
      g.goto top
      g.new_label.set!  # break takes here
      g.pop_modifiers
    end
  end

  it "compiles break in a control" do
    ruby = <<-EOC
      while true do
        a=12
        break
      end
    EOC

    sexp = s(:while, s(:true),
             s(:block,
               s(:lasgn, :a, s(:fixnum, 12)),
               s(:break)), true)


    # TODO: ditto
    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers

      top = g.new_label
      fin = g.new_label
      g.redo = g.new_label
      g.break = g.new_label

      top.set!
      g.push :true
      g.gif fin
      g.redo.set!
      g.push 12
      g.set_local 0
      g.pop
      g.push :nil
      g.goto g.break
      g.pop
      g.goto top

      fin.set!
      g.push :nil

      g.break.set!

      g.pop_modifiers
    end
  end

  it "compiles break in a block" do
    ruby = <<-EOC
      go { a=12; break }
    EOC

    sexp = s(:iter,
             s(:call, nil, :go, s(:arglist)),
             nil,
             s(:block,
               s(:lasgn, :a, s(:fixnum, 12)),
               s(:break)))

    # TODO: same here, no 12
    sexp.should == parse(ruby)

    gen sexp do |g|
      iter = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12
        d.set_local_depth 0, 0
        d.pop

        d.push :nil
        d.break_raise

        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.create_block iter
      g.passed_block do
        g.send_with_block :go, 0, true
      end
    end
  end

  it "compiles an unexpected break" do
    ruby = <<-EOC
      break
    EOC

    sexp = s(:break)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push :nil
      g.pop
      g.push_const :Compile
      g.send :__unexpected_break__, 0
    end
  end

  it "compiles redo in a while" do
    ruby = <<-EOC
      while true do
        a=12
        redo
      end
    EOC

    sexp = s(:while,
             s(:true),
             s(:block,
               s(:lasgn, :a, s(:fixnum, 12)),
               s(:redo)),
             true)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push_modifiers

      top = g.new_label
      fin = g.new_label
      g.redo = g.new_label

      top.set!
      g.push :true
      g.gif fin
      g.redo.set!
      g.push 12
      g.set_local 0
      g.pop
      g.goto g.redo
      g.pop
      g.goto top

      fin.set!
      g.push :nil

      # Break
      g.new_label.set!

      g.pop_modifiers
    end
  end

  it "compiles redo in a block" do
    ruby = <<-EOC
      go do
        a=12
        redo
      end
    EOC

    sexp = s(:iter,
             s(:call, nil, :go, s(:arglist)),
             nil,
             s(:block,
               s(:lasgn, :a, s(:fixnum, 12)),
               s(:redo)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      iter = description do |d|
        d.pop
        d.push_modifiers
        d.redo = d.new_label
        d.redo.set!
        d.push 12
        d.set_local_depth 0, 0
        d.pop
        d.goto d.redo
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.create_block iter
      g.passed_block do
        g.send_with_block :go, 0, true
      end
    end
  end

  it "compiles an invalid redo" do
    ruby = <<-EOC
      redo
    EOC

    sexp = s(:redo)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push :self
      g.push_const :LocalJumpError
      g.push_literal "redo used in invalid context"
      g.send :raise, 2, true
    end
  end

  it "compiles a simple case" do
    ruby = <<-EOC
      case true
      when Fixnum then 12
      end
    EOC

    sexp = s(:case, s(:true),
             s(:when, s(:array, s(:const, :Fixnum)),
                 s(:fixnum, 12)),
             nil)

    sexp.should == parse(ruby)

    gen sexp do |g|
      nxt = g.new_label
      fin = g.new_label

      g.push :true
      g.dup

      g.push_const :Fixnum
      g.swap
      g.send :===, 1

      g.gif nxt

      g.pop
      g.push 12
      g.goto fin

      nxt.set!
      g.pop
      g.push :nil
      fin.set!
    end
  end

  it "compiles a case with multiple conditions" do
    ruby = <<-EOC
      case true
      when Fixnum, String then 12
      end
    EOC

    sexp = s(:case, s(:true),
             s(:when, s(:array, s(:const, :Fixnum), s(:const, :String)),
                 s(:fixnum, 12)),
             nil)

    sexp.should == parse(ruby)

    gen sexp do |g|
      nxt  = g.new_label
      fin  = g.new_label
      body = g.new_label

      g.push :true

      g.dup
      g.push_const :Fixnum
      g.swap
      g.send :===, 1
      g.git body

      g.dup
      g.push_const :String
      g.swap
      g.send :===, 1
      g.git body
      g.goto nxt

      body.set!
      g.pop
      g.push 12
      g.goto fin

      nxt.set!
      g.pop
      g.push :nil
      fin.set!
    end

  end

  it "compiles a case with multiple whens" do
    ruby = <<-EOC
      case true
      when Fixnum then 12
      when String then 13
      end
    EOC

    sexp = s(:case, s(:true),
             s(:when, s(:array, s(:const, :Fixnum)),
                 s(:fixnum, 12)),
             s(:when, s(:array, s(:const, :String)),
                 s(:fixnum, 13)),
             nil)

    sexp.should == parse(ruby)

    gen sexp do |g|
      nxt1 = g.new_label
      nxt2 = g.new_label
      fin =  g.new_label

      g.push :true

      g.dup
      g.push_const :Fixnum
      g.swap
      g.send :===, 1

      g.gif nxt1

      g.pop
      g.push 12
      g.goto fin

      nxt1.set!
      g.dup
      g.push_const :String
      g.swap
      g.send :===, 1

      g.gif nxt2

      g.pop
      g.push 13
      g.goto fin

      nxt2.set!
      g.pop
      g.push :nil
      fin.set!
    end

  end

  it "compiles a case with an else" do
    ruby = <<-EOC
      case true
      when Fixnum then 12
      else 14
      end
    EOC

    sexp = s(:case, s(:true),
             s(:when, s(:array, s(:const, :Fixnum)),
                 s(:fixnum, 12)),
             s(:fixnum, 14))

    sexp.should == parse(ruby)

    gen sexp do |g|
      nxt = g.new_label
      fin = g.new_label

      g.push :true
      g.dup

      g.push_const :Fixnum
      g.swap
      g.send :===, 1

      g.gif nxt

      g.pop
      g.push 12
      g.goto fin

      nxt.set!
      g.pop
      g.push 14
      fin.set!
    end

  end

  it "compiles a case without an argument" do
    ruby = <<-EOC
      case
      when false  then "foo"
      when nil    then "foo"
      when 1 == 2 then "bar"
                  else "baz"
      end
    EOC

    sexp = parse(ruby)

    gen sexp do |g|
      fin   = g.new_label
      cond2 = g.new_label
      cond3 = g.new_label
      cond4 = g.new_label

      g.push :false
      g.gif cond2
      g.push_literal "foo"
      g.string_dup
      g.goto fin

      cond2.set!

      g.push :nil
      g.gif cond3
      g.push_literal "foo"
      g.string_dup
      g.goto fin

      cond3.set!

      g.push 1
      g.push 2
      g.meta_send_op_equal
      g.gif cond4
      g.push_literal "bar"
      g.string_dup
      g.goto fin

      cond4.set!

      g.push_literal "baz"
      g.string_dup

      fin.set!
    end
  end

  it "compiles a case without an argument, branch with multiple conds" do
    # TODO: again: huh? this whole example is borked
    ruby = <<-EOC
      case
      when false      then "foo"
      when nil        then "foo"
      when 1 == 2, 13 then "bar"
      end
    EOC

    sexp = parse(ruby)

    gen sexp do |g|
      fin =   g.new_label
      cond2 = g.new_label
      cond3 = g.new_label
      cond4 = g.new_label

      g.push :false
      g.gif cond2
      g.push_literal "foo"
      g.string_dup
      g.goto fin

      cond2.set!

      g.push :nil
      g.gif cond3
      g.push_literal "foo"
      g.string_dup
      g.goto fin

      cond3.set!

      body = g.new_label
      g.push 1
      g.push 2
      g.meta_send_op_equal
      g.git body
      g.push 13
      g.git body
      g.goto cond4

      body.set!
      g.push_literal "bar"
      g.string_dup
      g.goto fin

      cond4.set!

      g.push :nil

      fin.set!
    end
  end


  it "compiles a case with a splat" do
    ruby = <<-EOC
      case true
      when *things then
        12
      end
    EOC

    sexp = s(:case, s(:true),
             s(:when, s(:array, s(:when,
                                  s(:call, nil, :things, s(:arglist)), nil)),
                 s(:fixnum, 12)),
             nil)

    sexp.should == parse(ruby)

    gen sexp do |g|
      body = g.new_label
      nxt =  g.new_label
      fin =  g.new_label

      g.push :true

      g.dup
      g.push :self
      g.send :things, 0, true
      g.cast_array
      g.swap
      g.send :__matches_when__, 1
      g.git body

      g.goto nxt

      body.set!
      g.pop
      g.push 12
      g.goto fin

      nxt.set!
      g.pop
      g.push :nil
      fin.set!
    end
  end

  it "compiles a case with normal conditions and a splat" do
    ruby = <<-EOC
      case true
      when String, *things then
        12
      end
    EOC

    sexp = s(:case, s(:true),
             s(:when, s(:array, s(:const, :String),
                          s(:when, s(:call, nil, :things, s(:arglist)), nil)),
                 s(:fixnum, 12)),
             nil)

    sexp.should == parse(ruby)

    gen sexp do |g|
      body = g.new_label
      nxt =  g.new_label
      fin =  g.new_label

      g.push :true

      g.dup
      g.push_const :String
      g.swap
      g.send :===, 1
      g.git body

      g.dup
      g.push :self
      g.send :things, 0, true
      g.cast_array
      g.swap
      g.send :__matches_when__, 1
      g.git body

      g.goto nxt

      body.set!
      g.pop
      g.push 12
      g.goto fin

      nxt.set!
      g.pop
      g.push :nil
      fin.set!
    end
  end

  it "compiles a case with normal conditions and a splatted array" do
    ruby = <<-EOC
      case true
      when String, *%w(foo bar baz) then
        12
      end
    EOC

    sexp = s(:case, s(:true),
             s(:when, s(:array, s(:const, :String),
                          s(:when,
                            s(:array,
                              s(:str, "foo"),
                              s(:str, "bar"),
                              s(:str, "baz")), nil)),
                 s(:fixnum, 12)),
             nil)

    sexp.should == parse(ruby)

    gen sexp do |g|
      body = g.new_label
      nxt  = g.new_label
      fin  = g.new_label

      g.push :true

      g.dup
      g.push_const :String
      g.swap
      g.send :===, 1
      g.git body

      g.dup
      g.push_literal "foo"
      g.string_dup
      g.swap
      g.send :===, 1
      g.git body

      g.dup
      g.push_literal "bar"
      g.string_dup
      g.swap
      g.send :===, 1
      g.git body

      g.dup
      g.push_literal "baz"
      g.string_dup
      g.swap
      g.send :===, 1
      g.git body

      g.goto nxt

      body.set!
      g.pop
      g.push 12
      g.goto fin

      nxt.set!
      g.pop
      g.push :nil
      fin.set!
    end
  end

  it "compiles 'return'" do
    ruby = <<-EOC
      return
    EOC

    sexp = s(:return)

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push :nil
      g.ret
    end
  end

  it "compiles 'return 12'" do
    ruby = <<-EOC
      return 12
    EOC

    sexp = s(:return, s(:fixnum, 12))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push 12
      g.ret
    end
  end

  it "compiles 'begin; 12; rescue String; return 13; end'" do
    ruby = <<-EOC
      begin; 12; rescue String; return 13; end
    EOC

    sexp = s(:rescue, s(:fixnum, 12),
             s(:resbody, s(:array, s(:const, :String)), # HUH? FIX
               s(:return, s(:fixnum, 13))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      exc_start  = g.new_label
      exc_handle = g.new_label
      fin        = g.new_label
      rr         = g.new_label
      last       = g.new_label
      body       = g.new_label

      g.push_modifiers

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
      g.push 13
      g.ret
      g.clear_exception

      g.goto last

      rr.set!

      g.push_exception
      g.raise_exc

      fin.set!

      last.set!

      g.pop_modifiers
    end
  end

  it "compiles return in a block" do
    ruby = <<-EOC
      go { return 12 }
    EOC

    sexp = s(:iter,
             s(:call, nil, :go, s(:arglist)),
             nil,
             s(:return, s(:fixnum, 12)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      iter = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12

        d.return_raise

        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.create_block iter
      g.passed_block do
        g.send_with_block :go, 0, true
      end
    end
  end

  it "compiles 'return *1'" do
    ruby = <<-EOC
      return *1
    EOC

    sexp = s(:return,
             s(:svalue,
               s(:splat, s(:fixnum, 1))))
    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push 1 # 1
      g.cast_array # [1]
      g.dup # [1] [1]
      g.send :size, 0 # size [1]
      g.push 1 # 1 size [1]
      g.send :>, 1 # bool [1]

      more_than_one = g.new_label
      g.git more_than_one # [1]

      g.push 0 # 0 [1]
      g.send :at, 1

      more_than_one.set!
      g.ret
    end
  end

  it "compiles 'return 1, 2, *c'" do
    ruby = <<-EOC
      return 1, 2, *c
    EOC

    sexp = s(:return,
             s(:array,
               s(:fixnum, 1),
               s(:fixnum, 2),
               s(:splat, s(:call, nil, :c, s(:arglist)))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push 1
      g.push 2
      g.make_array 2

      g.push :self
      g.send :c, 0, true
      g.cast_array

      g.send :+, 1
      g.ret
    end
  end
end
