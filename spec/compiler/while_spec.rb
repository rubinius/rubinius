require File.dirname(__FILE__) + '/../spec_helper'

describe "A While node" do
  pre_while_sexp = [
    :while,
     [:call, nil, :a, [:arglist]],
     [:call, [:call, nil, :b, [:arglist]], :+, [:arglist, [:lit, 1]]],
     true
  ]

  pre_while = lambda do |g|
    top    = g.new_label
    rdo    = g.new_label
    brk    = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!
    g.push :self
    g.send :a, 0, true
    g.gif bottom

    rdo.set!
    g.push :self
    g.send :b, 0, true
    g.push 1
    g.send :+, 1, false
    g.pop

    g.check_interrupts
    g.goto top

    bottom.set!
    g.push :nil

    brk.set!
    g.pop_modifiers
  end

  relates <<-ruby do
      while a
        b + 1
      end
    ruby

    compile(&pre_while)
  end

  relates <<-ruby do
      a = x
      while a.b
        1
      end
    ruby

    compile do |g|
      top    = g.new_label
      rdo    = g.new_label
      brk    = g.new_label
      bottom = g.new_label

      g.push :self
      g.send :x, 0, true
      g.set_local 0
      g.pop

      g.push_modifiers
      top.set!

      g.push_local 0
      g.send :b, 0, false
      g.gif bottom

      rdo.set!
      g.push 1
      g.pop

      g.check_interrupts
      g.goto top

      bottom.set!
      g.push :nil

      brk.set!
      g.pop_modifiers
    end
  end

  relates "b + 1 while a" do
    compile(&pre_while)
  end

  relates <<-ruby do
      until not a
        b + 1
      end
    ruby

    compile(&pre_while)
  end

  relates "b + 1 until not a" do
    compile(&pre_while)
  end

  post_while_sexp = [
    :while,
     [:call, nil, :a, [:arglist]],
     [:call, [:call, nil, :b, [:arglist]], :+, [:arglist, [:lit, 1]]],
     false
  ]

  post_while = lambda do |g|
    top    = g.new_label
    rdo    = g.new_label
    brk    = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!

    g.push :self
    g.send :b, 0, true
    g.push 1
    g.send :+, 1, false
    g.pop

    rdo.set!

    g.push :self
    g.send :a, 0, true
    g.gif bottom

    g.check_interrupts
    g.goto top

    bottom.set!
    g.push :nil

    brk.set!
    g.pop_modifiers
  end

  relates <<-ruby do
      begin
        b + 1
      end while a
    ruby

    compile(&post_while)
  end

  relates <<-ruby do
      begin
        b + 1
      end until not a
    ruby

    compile(&post_while)
  end

  nil_condition_sexp = [:while, [:nil], [:call, nil, :a, [:arglist]], true]

  nil_condition = lambda do |g|
    top    = g.new_label
    rdo    = g.new_label
    brk    = g.new_label
    bottom = g.new_label

    g.push_modifiers

    top.set!
    g.push :nil
    g.gif bottom

    rdo.set!
    g.push :self
    g.send :a, 0, true
    g.pop

    g.check_interrupts
    g.goto top

    bottom.set!
    g.push :nil

    brk.set!
    g.pop_modifiers
  end

  relates "a while ()" do
    compile(&nil_condition)
  end

  relates <<-ruby do
      while ()
        a
      end
    ruby

    compile(&nil_condition)
  end

  relates "a until not ()" do
    compile(&nil_condition)
  end

  relates <<-ruby do
      until not ()
        a
      end
    ruby

    compile(&nil_condition)
  end

  relates <<-ruby do
      while 1
        2
        break :brk
      end
    ruby

    compile do |g|
      top    = g.new_label
      rdo    = g.new_label
      brk    = g.new_label
      bottom = g.new_label

      g.push_modifiers
      top.set!

      g.push 1
      g.gif bottom

      rdo.set!
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
  end

  relates <<-ruby do
      while 1
        begin
          2
          break :brk
        rescue
          3
        end
      end
    ruby

    compile do |g|
      top         = g.new_label
      retry_lbl   = g.new_label
      exc_lbl     = g.new_label
      noexc_lbl   = g.new_label
      rescue_lbl  = g.new_label
      reraise_lbl = g.new_label
      done        = g.new_label
      bottom      = g.new_label
      cur_brk_lbl = g.new_label
      break_lbl   = g.new_label

      g.push_modifiers
      top.set!              # 1

      g.push 1
      g.gif bottom

      # redo
      g.new_label.set!      # 2
      g.push_modifiers
      saved = g.save_exception

      retry_lbl.set!        # 3
      g.setup_unwind exc_lbl

      # redo
      g.new_label.set!      # 4
      g.push 2
      g.pop
      g.push_literal :brk
      g.goto cur_brk_lbl

      g.pop_unwind
      g.goto noexc_lbl

      cur_brk_lbl.set!      # 5
      g.pop_unwind

      g.restore_exception saved
      g.goto break_lbl

      exc_lbl.set!          # 6
      g.push_const :StandardError
      g.push_exception
      g.send :===, 1
      g.git rescue_lbl
      g.goto reraise_lbl

      rescue_lbl.set!       # 7
      g.push 3
      g.clear_exception
      g.goto done

      reraise_lbl.set!      # 8
      g.reraise

      noexc_lbl.set!        # 9
      done.set!             # 10
      g.restore_exception saved
      g.pop_modifiers

      g.pop
      g.check_interrupts
      g.goto top

      bottom.set!           # 11
      g.push :nil

      break_lbl.set!        # 12
      g.pop_modifiers
    end
  end

  relates <<-ruby do
      while 1
        begin
          2
        rescue
          3
          break :brk
        end
      end
    ruby

    compile do |g|
      top         = g.new_label
      retry_lbl   = g.new_label
      exc_lbl     = g.new_label
      noexc_lbl   = g.new_label
      rescue_lbl  = g.new_label
      reraise_lbl = g.new_label
      done        = g.new_label
      bottom      = g.new_label
      cur_brk_lbl = g.new_label
      break_lbl   = g.new_label

      g.push_modifiers
      top.set!              # 1

      g.push 1
      g.gif bottom

      # redo
      g.new_label.set!      # 2
      g.push_modifiers
      saved = g.save_exception
      retry_lbl.set!        # 3
      g.setup_unwind exc_lbl

      # redo
      g.new_label.set!      # 4
      g.push 2
      g.pop_unwind
      g.goto noexc_lbl

      exc_lbl.set!          # 5
      g.push_const :StandardError
      g.push_exception
      g.send :===, 1
      g.git rescue_lbl
      g.goto reraise_lbl

      rescue_lbl.set!       # 6
      g.push 3
      g.pop
      g.push_literal :brk
      g.goto cur_brk_lbl

      g.clear_exception
      g.goto done

      cur_brk_lbl.set!      # 7
      g.clear_exception

      g.restore_exception saved
      g.goto break_lbl

      reraise_lbl.set!      # 8
      g.reraise

      noexc_lbl.set!        # 9
      done.set!             # 10
      g.restore_exception saved
      g.pop_modifiers

      g.pop
      g.check_interrupts
      g.goto top

      bottom.set!           # 11
      g.push :nil

      break_lbl.set!        # 12
      g.pop_modifiers
    end
  end
end
