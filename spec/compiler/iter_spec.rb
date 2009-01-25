require File.dirname(__FILE__) + '/../spec_helper'

describe "An Iter node" do
  relates <<-ruby do
      a(b) do
        if b then
          true
        else
          c = false
          d { |x| c = true }
          c
        end
      end
    ruby

    parse do
      [:iter,
         [:call, nil, :a, [:arglist, [:call, nil, :b, [:arglist]]]],
         nil,
         [:if,
          [:call, nil, :b, [:arglist]],
          [:true],
          [:block,
           [:lasgn, :c, [:false]],
           [:iter,
            [:call, nil, :d, [:arglist]],
            [:lasgn, :x],
            [:lasgn, :c, [:true]]],
           [:lvar, :c]]]]
    end

    compile do |g|
      g.push :self
      g.push :self
      g.send :b, 0, true

      in_block_send :a, 0, 1 do |d|
        f = d.new_label
        bottom = d.new_label

        d.push :self
        d.send :b, 0, true
        d.gif f
        d.push :true
        d.goto bottom
        f.set!
        d.push :false
        d.set_local_depth 0, 0
        d.pop
        d.push :self

        d.in_block_send :d, 1, 0, true, 0, true do |d2|
          d2.push :true
          d2.set_local_depth 1, 0
        end

        d.pop
        d.push_local_depth 0, 0
        bottom.set!
      end
    end
  end

  relates "loop { break 42 if true }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:if, [:true], [:break, [:lit, 42]], nil]]
    end

    compile do |g|
      break_value = 42

      top   = g.new_label
      cond  = g.new_label
      rtry  = g.new_label
      brk   = g.new_label

      g.push_modifiers

      top.set!
      g.push :true
      g.gif cond

      g.push break_value
      g.goto brk
      g.goto rtry # TODO: only used when there is a retry statement

      cond.set!
      g.push :nil

      rtry.set!
      g.pop
      g.goto top

      brk.set!

      g.pop_modifiers
    end
  end

  relates "loop { break if true }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:if, [:true], [:break], nil]]
    end

    compile do |g|
      break_value = :nil # TODO: refactor later

      top   = g.new_label
      cond  = g.new_label
      rtry  = g.new_label
      brk   = g.new_label

      g.push_modifiers

      top.set!
      g.push :true
      g.gif cond

      g.push break_value
      g.goto brk
      g.goto rtry # TODO: only used when there is a retry statement

      cond.set!
      g.push :nil

      rtry.set!
      g.pop
      g.goto top

      brk.set!

      g.pop_modifiers
    end
  end

  relates "loop { break *[nil] }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:break, [:svalue, [:splat, [:array, [:nil]]]]]]
    end

    # break splat
  end

  relates "loop { break [*[1]] }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:break, [:array, [:splat, [:array, [:lit, 1]]]]]]
    end

    # splat break array
  end

  relates "a.each { |x| b.each { |y| x = (x + 1) } if true }" do
    parse do
      [:iter,
       [:call, [:call, nil, :a, [:arglist]], :each, [:arglist]],
       [:lasgn, :x],
       [:if,
        [:true],
        [:iter,
         [:call, [:call, nil, :b, [:arglist]], :each, [:arglist]],
         [:lasgn, :y],
         [:lasgn, :x, [:call, [:lvar, :x], :+, [:arglist, [:lit, 1]]]]],
        nil]]
    end

    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.in_block_send :each, 1, 0, false, 0, false do |d|
        t = d.new_label
        f = d.new_label

        d.push :true
        d.gif f

        d.push :self
        d.send :b, 0, true
        d.in_block_send :each, 1, 0, false, 0, true do |d2|
          d2.push_local_depth 1, 0
          d2.push 1
          d2.meta_send_op_plus
          d2.set_local_depth 1, 0
        end

        d.goto t
        f.set!
        d.push :nil
        t.set!
      end
    end
  end


  relates "a.each { |x| b.each { |y| c = (c + 1) } if true }" do
    parse do
      [:iter,
       [:call, [:call, nil, :a, [:arglist]], :each, [:arglist]],
       [:lasgn, :x],
       [:if,
        [:true],
        [:iter,
         [:call, [:call, nil, :b, [:arglist]], :each, [:arglist]],
         [:lasgn, :y],
         [:lasgn, :c, [:call, [:lvar, :c], :+, [:arglist, [:lit, 1]]]]],
        nil]]
    end

    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.in_block_send :each, 1, 0, false, 0, false do |d|
        t = d.new_label
        f = d.new_label

        d.push :true
        d.gif f

        d.push :self
        d.send :b, 0, true
        d.in_block_send :each, 1, 0, false, 0, true do |d2|
          d2.push_local_depth 0, 1
          d2.push 1
          d2.meta_send_op_plus
          d2.set_local_depth 0, 1
        end

        d.goto t
        f.set!
        d.push :nil
        t.set!
      end
    end
  end

  relates <<-ruby do
      a.each do |x|
        if true then
          c = 0
          b.each { |y| c = (c + 1) }
        end
      end
    ruby

    parse do
      [:iter,
       [:call, [:call, nil, :a, [:arglist]], :each, [:arglist]],
       [:lasgn, :x],
       [:if,
        [:true],
        [:block,
         [:lasgn, :c, [:lit, 0]],
         [:iter,
          [:call, [:call, nil, :b, [:arglist]], :each, [:arglist]],
          [:lasgn, :y],
          [:lasgn, :c, [:call, [:lvar, :c], :+, [:arglist, [:lit, 1]]]]]],
        nil]]
    end

    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.in_block_send :each, 1, 0, false, 0, false do |d|
        t = d.new_label
        f = d.new_label

        d.push :true
        d.gif f

        d.push 0
        d.set_local_depth 0, 1
        d.pop

        d.push :self
        d.send :b, 0, true
        d.in_block_send :each, 1, 0, false, 0, true do |d2|
          d2.push_local_depth 1, 1
          d2.push 1
          d2.meta_send_op_plus
          d2.set_local_depth 1, 1
        end

        d.goto t
        f.set!
        d.push :nil
        t.set!
      end
    end
  end

  relates <<-ruby do
      data.each do |x, y|
        a = 1
        b = a
        b = a = x
      end
    ruby

    parse do
      [:iter,
       [:call, [:call, nil, :data, [:arglist]], :each, [:arglist]],
       [:masgn, [:array, [:lasgn, :x], [:lasgn, :y]]],
       [:block,
        [:lasgn, :a, [:lit, 1]],
        [:lasgn, :b, [:lvar, :a]],
        [:lasgn, :b, [:lasgn, :a, [:lvar, :x]]]]]
    end

    compile do |g|
      g.push :self
      g.send :data, 0, true
      g.in_block_send :each, 2, 0, false do |d|
        d.push 1
        d.set_local_depth 0, 2
        d.pop

        d.push_local_depth 0, 2
        d.set_local_depth 0, 3
        d.pop

        d.push_local_depth 0, 0
        d.set_local_depth 0, 2
        d.set_local_depth 0, 3
      end
    end
  end

  relates <<-ruby do
      a do
        v = nil
        assert_block(full_message) do
          begin
            yield
          rescue Exception => v
            break
          end
        end
      end
    ruby

    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       nil,
       [:block,
        [:lasgn, :v, [:nil]],
        [:iter,
         [:call,
          nil,
          :assert_block,
          [:arglist, [:call, nil, :full_message, [:arglist]]]],
         nil,
         [:rescue,
          [:yield],
          [:resbody,
           [:array, [:const, :Exception], [:lasgn, :v, [:gvar, :$!]]],
           [:break]]]]]]
    end

    compile do |g|
      g.push :self
      g.in_block_send :a do |d|
        d.push :nil
        d.set_local_depth 0, 0
        d.pop

        d.push :self
        d.push :self
        d.send :full_message, 0, true

        d.in_block_send :assert_block, 0, 1, true, 0, true do |d2|
          d2.in_rescue :Exception, 1 do |section|
            case section
            when :body then
              d2.push_block
              d2.meta_send_call 0
            when :Exception then
              d2.push_exception
              d2.set_local_depth 1, 0
              d2.push :nil

              d2.break_raise
            end
          end
        end
      end
    end
  end

  relates <<-ruby do
      t = 0
      ns.each { |n| t += n }
    ruby

    parse do
      [:block,
       [:lasgn, :t, [:lit, 0]],
       [:iter,
        [:call, [:call, nil, :ns, [:arglist]], :each, [:arglist]],
        [:lasgn, :n],
        [:lasgn, :t, [:call, [:lvar, :t], :+, [:arglist, [:lvar, :n]]]]]]
    end

    compile do |g|
      g.push 0
      g.set_local 0
      g.pop

      g.push :self
      g.send :ns, 0, true

      in_block_send :each, 1, 0, false, 1 do |d|
        d.push_local 0
        d.push_local_depth 0, 0
        d.meta_send_op_plus
        d.set_local 0
      end
    end
  end

  relates "a (1) {|c|d}" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist, [:lit, 1]]],
       [:lasgn, :c],
       [:call, nil, :d, [:arglist]]]
    end

    compile do |g|
      g.push :self
      g.push 1
      in_block_send :a, 1, 1 do |d|
        d.push :self
        d.send :d, 0, true
      end
    end
  end

  relates <<-ruby do
      as.each { |a|
        b += a.b(false) }
    ruby

    parse do
      [:iter,
       [:call, [:call, nil, :as, [:arglist]], :each, [:arglist]],
       [:lasgn, :a],
       [:lasgn,
        :b,
        [:call,
         [:lvar, :b],
         :+,
         [:arglist, [:call, [:lvar, :a], :b, [:arglist, [:false]]]]]]]
    end

    compile do |g|
      g.push :self
      g.send :as, 0, true
      in_block_send :each, 1, 0, false do |d|
        d.push_local_depth 0, 1
        d.push_local_depth 0, 0
        d.push :false
        d.send :b, 1, false
        d.meta_send_op_plus
        d.set_local_depth 0, 1
      end
    end
  end

  relates "3.downto(1) { |n| puts(n.to_s) }" do
    parse do
      [:iter,
       [:call, [:lit, 3], :downto, [:arglist, [:lit, 1]]],
       [:lasgn, :n],
       [:call, nil, :puts, [:arglist, [:call, [:lvar, :n], :to_s, [:arglist]]]]]
    end

    compile do |g|
      g.push 3
      g.push 1
      in_block_send :downto, 1, 1, false do |d|
        d.push :self
        d.push_local_depth 0, 0
        d.send :to_s, 0, false
        d.send :puts, 1, true
      end
    end
  end

  relates <<-ruby do
      array = [1, 2, 3]
      array.each { |x| puts(x.to_s) }
    ruby

    parse do
      [:block,
       [:lasgn, :array, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
       [:iter,
        [:call, [:lvar, :array], :each, [:arglist]],
        [:lasgn, :x],
        [:call, nil, :puts, [:arglist, [:call, [:lvar, :x], :to_s, [:arglist]]]]]]
    end

    compile do |g|
      g.push 1
      g.push 2
      g.push 3
      g.make_array 3
      g.set_local 0
      g.pop

      g.push_local 0
      in_block_send :each, 1, 0, false, 1 do |d|
        d.push :self
        d.push_local_depth 0, 0
        d.send :to_s, 0, false
        d.send :puts, 1, true
      end
    end
  end

  relates <<-ruby do
      array1 = [1, 2, 3]
      array2 = [4, 5, 6, 7]
      array1.each do |x|
        array2.each do |y|
          puts(x.to_s)
          puts(y.to_s)
        end
      end
    ruby

    parse do
      [:block,
       [:lasgn, :array1, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
       [:lasgn, :array2, [:array, [:lit, 4], [:lit, 5], [:lit, 6], [:lit, 7]]],
       [:iter,
        [:call, [:lvar, :array1], :each, [:arglist]],
        [:lasgn, :x],
        [:iter,
         [:call, [:lvar, :array2], :each, [:arglist]],
         [:lasgn, :y],
         [:block,
          [:call, nil, :puts, [:arglist, [:call, [:lvar, :x], :to_s, [:arglist]]]],
          [:call,
           nil,
           :puts,
           [:arglist, [:call, [:lvar, :y], :to_s, [:arglist]]]]]]]]
    end

    compile do |g|
      g.push 1
      g.push 2
      g.push 3
      g.make_array 3
      g.set_local 0
      g.pop

      g.push 4
      g.push 5
      g.push 6
      g.push 7
      g.make_array 4
      g.set_local 1
      g.pop

      g.push_local 0
      in_block_send :each, 1, 0, false, 2 do |d|
        d.push_local 1
        d.in_block_send :each, 1, 0, false, 2, true do |d2|
          d2.push :self
          d2.push_local_depth 1, 0
          d2.send :to_s, 0, false
          d2.send :puts, 1, true
          d2.pop

          d2.push :self
          d2.push_local_depth 0, 0
          d2.send :to_s, 0, false
          d2.send :puts, 1, true
        end
      end
    end
  end

  relates "loop { }" do
    parse do
      [:iter, [:call, nil, :loop, [:arglist]], nil]
    end

    compile do |g|
      top = g.new_label
      bottom = g.new_label

      g.push_modifiers
      top.set!
      g.goto top
      bottom.set!
      g.pop_modifiers
    end
  end

  relates "a { |b, c| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:lasgn, :b], [:lasgn, :c]]],
       [:call, nil, :p, [:arglist, [:lvar, :c]]]]
    end

    compile do |g|
      g.push :self
      in_block_send :a, 2 do |d|
        d.push :self
        d.push_local_depth 0, 1
        d.send :p, 1, true
      end
    end
  end

  relates "a { |b, c, *| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:lasgn, :b], [:lasgn, :c], [:splat]]],
       [:call, nil, :p, [:arglist, [:lvar, :c]]]]
    end

    compile do |g|
      g.push :self
      in_block_send :a, 2 do |d|
        d.push :self
        d.push_local_depth 0, 1
        d.send :p, 1, true
      end
    end
  end

  relates "a { |b, c, *d| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:lasgn, :b], [:lasgn, :c], [:splat, [:lasgn, :d]]]],
       [:call, nil, :p, [:arglist, [:lvar, :c]]]]
    end

    compile do |g|
      g.push :self
      in_block_send :a, [2, 2] do |d|
        d.push :self
        d.push_local_depth 0, 1
        d.send :p, 1, true
      end
    end
  end

  relates "a { |*| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:splat]]],
       [:call, nil, :p, [:arglist, [:call, nil, :c, [:arglist]]]]]
    end

    compile do |g|
      g.push :self
      in_block_send :a, -2 do |d|
        d.push :self
        d.push :self
        d.send :c, 0, true
        d.send :p, 1, true
      end
    end
  end

  relates "a { |*c| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:splat, [:lasgn, :c]]]],
       [:call, nil, :p, [:arglist, [:lvar, :c]]]]
    end

    compile do |g|
      g.push :self
      in_block_send :a, -1 do |d|
        d.push :self
        d.push_local_depth 0, 0
        d.send :p, 1, true
      end
    end
  end

  relates <<-ruby do
      a do |x|
        b do |x|
          puts x
        end
      end
    ruby

    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:lasgn, :x],
       [:iter,
        [:call, nil, :b, [:arglist]],
        [:lasgn, :x],
        [:call, nil, :puts, [:arglist, [:lvar, :x]]]]]
    end

    compile do |g|
      g.push :self
      g.in_block_send :a, 1 do |d|
        d.push :self
        d.in_block_send :b, 1, 0, true, 0, true, 1 do |d2|
          d2.push :self
          d2.push_local_depth 1, 0
          d2.send :puts, 1, true
        end
      end
    end
  end

  relates "1.upto(3) { |n| puts(n.to_s) }" do
    parse do
      [:iter,
       [:call, [:lit, 1], :upto, [:arglist, [:lit, 3]]],
       [:lasgn, :n],
       [:call, nil, :puts, [:arglist, [:call, [:lvar, :n], :to_s, [:arglist]]]]]
    end

    compile do |g|
      g.push 1
      g.push 3
      in_block_send :upto, 1, 1, false do |d|
        d.push :self
        d.push_local_depth 0, 0
        d.send :to_s, 0, false
        d.send :puts, 1, true
      end
    end
  end

  relates <<-ruby do
      argl = 10
      while (argl >= 1) do
        puts("hello")
        argl = (argl - 1)
      end
    ruby

    parse do
      [:block,
       [:lasgn, :argl, [:lit, 10]],
       [:while,
        [:call, [:lvar, :argl], :>=, [:arglist, [:lit, 1]]],
        [:block,
         [:call, nil, :puts, [:arglist, [:str, "hello"]]],
         [:lasgn, :argl, [:call, [:lvar, :argl], :-, [:arglist, [:lit, 1]]]]],
        true]]
    end

    compile do |g|
      top    = g.new_label
      f      = g.new_label
      dunno1 = g.new_label
      dunno2 = g.new_label
      bottom = g.new_label

      g.push 10
      g.set_local 0
      g.pop

      g.push_modifiers
      top.set!

      g.push_local 0
      g.push 1
      g.send :>=, 1, false
      g.gif f

      dunno1.set!

      g.push :self
      g.push_literal "hello"
      g.string_dup
      g.send :puts, 1, true
      g.pop

      g.push_local 0
      g.push 1
      g.meta_send_op_minus
      g.set_local 0
      g.pop

      g.goto top

      f.set!
      g.push :nil

      bottom.set!

      g.pop_modifiers
    end
  end

  relates <<-ruby do
      a.b do |c, d|
        unless e.f(c) then
          g = false
          d.h { |x, i| g = true }
        end
      end
    ruby

    parse do
      [:iter,
       [:call, [:call, nil, :a, [:arglist]], :b, [:arglist]],
       [:masgn, [:array, [:lasgn, :c], [:lasgn, :d]]],
       [:if,
        [:call, [:call, nil, :e, [:arglist]], :f, [:arglist, [:lvar, :c]]],
        nil,
        [:block,
         [:lasgn, :g, [:false]],
         [:iter,
          [:call, [:lvar, :d], :h, [:arglist]],
          [:masgn, [:array, [:lasgn, :x], [:lasgn, :i]]],
          [:lasgn, :g, [:true]]]]]]
    end

    compile do |g|
      g.push :self
      g.send :a, 0, true

      g.in_block_send :b, 2, 0, false do |d|
        f = d.new_label
        t = d.new_label

        d.push :self
        d.send :e, 0, true
        d.push_local_depth 0, 0
        d.send :f, 1, false
        d.git f

        d.push :false
        d.set_local_depth 0, 2
        d.pop

        d.push_local_depth 0, 1

        d.in_block_send :h, 2, 0, false, 0, true do |d2|
          d2.push :true
          d2.set_local_depth 1, 2
        end

        d.goto t
        f.set!
        d.push :nil
        t.set!
      end
    end
  end

  relates "loop { next if false }" do
    parse do
      [:iter,
        [:call, nil, :loop, [:arglist]], nil, [:if, [:false], [:next], nil]]
    end

    compile do |g|
      top    = g.new_label
      bottom = g.new_label
      dunno1 = g.new_label
      f      = g.new_label

      g.push_modifiers
      top.set!

      g.push :false
      g.gif f
      g.goto top
      g.goto dunno1

      f.set!

      g.push :nil

      dunno1.set!
      g.pop
      g.goto top

      bottom.set!

      g.pop_modifiers
    end
  end

  relates "loop { next 42 if false }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:if, [:false], [:next, [:lit, 42]], nil]]
    end

    # next arg
  end

  relates "loop { next *[1] }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:next, [:svalue, [:splat, [:array, [:lit, 1]]]]]]
    end

    compile do |g|
      g.splatted_array

      g.push :self
      g.push_const :LocalJumpError
      g.push_literal "next used in invalid context"
      g.send :raise, 2, true
    end
  end

  relates "loop { next [*[1]] }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:next, [:array, [:splat, [:array, [:lit, 1]]]]]]
    end

    compile do |g|
      g.array_of_splatted_array

      g.push :self
      g.push_const :LocalJumpError
      g.push_literal "next used in invalid context"
      g.send :raise, 2, true
    end
  end

  relates "loop { redo if false }" do
    parse do
      [:iter,
        [:call, nil, :loop, [:arglist]], nil, [:if, [:false], [:redo], nil]]
    end

    compile do |g|
      top = g.new_label
      f = g.new_label
      t = g.new_label
      bottom = g.new_label

      g.push_modifiers

      top.set!

      g.push :false
      g.gif f
      g.goto top
      g.goto t

      f.set!

      g.push :nil

      t.set!

      g.pop
      g.goto top

      bottom.set!

      g.pop_modifiers
    end
  end

  relates "loop { retry }" do
    parse do
      [:iter, [:call, nil, :loop, [:arglist]], nil, [:retry]]
    end

    # retry
  end

  relates "go { return 12 }" do
    parse do
      [:iter,
        [:call, nil, :go, [:arglist]], nil, [:return, [:lit, 12]]]
    end

    compile do |g|
      iter = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12

        d.return_raise

        d.pop_modifiers
        d.ret
      end

      g.passed_block do
        g.push :self
        g.create_block iter
        g.send_with_block :go, 0, true
      end
    end
  end

  relates <<-ruby do
      go do
        name = 12
        name
      end
    ruby

    parse do
      [:iter,
       [:call, nil, :go, [:arglist]],
       nil,
       [:block, [:lasgn, :name, [:lit, 12]], [:lvar, :name]]]
    end

    compile do |g|
      iter = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12
        d.set_local_depth 0, 0
        d.pop
        d.push_local_depth 0, 0
        d.pop_modifiers
        d.ret
      end

      g.passed_block do
        g.push :self
        g.create_block iter
        g.send_with_block :go, 0, true
      end
    end
  end

  relates <<-ruby do
      go do
        name = 12
        go do
          name
        end
      end
    ruby

    parse do
      [:iter,
       [:call, nil, :go, [:arglist]],
       nil,
       [:block,
        [:lasgn, :name, [:lit, 12]],
        [:iter, [:call, nil, :go, [:arglist]], nil, [:lvar, :name]]]]
    end

    compile do |g|
      iter = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12
        d.set_local_depth 0, 0
        d.pop

        i2 = description do |j|
          j.pop
          j.push_modifiers
          j.new_label.set! # redo
          j.push_local_depth 1, 0
          j.pop_modifiers
          j.ret
        end

        d.break_rescue do
          d.push :self
          d.create_block i2
          d.send_with_block :go, 0, true
        end
        d.pop_modifiers
        d.ret
      end

      g.passed_block do
        g.push :self
        g.create_block iter
        g.send_with_block :go, 0, true
      end
    end
  end

  relates "break" do
    parse do
      [:break]
    end

    compile do |g|
      g.push :nil
      g.pop
      g.push_const :Compile
      g.send :__unexpected_break__, 0
    end
  end

  relates "redo" do
    parse do
      [:redo]
    end

    compile do |g|
      g.push :self
      g.push_const :LocalJumpError
      g.push_literal "redo used in invalid context"
      g.send :raise, 2, true
    end
  end

  relates "retry" do
    parse do
      [:retry]
    end

    compile do |g|
      g.push :self
      g.push_const :LocalJumpError
      g.push_literal "retry used in invalid context"
      g.send :raise, 2, true
    end
  end

  relates "next" do
    parse do
      [:next]
    end

    compile do |g|
      g.push :self
      g.push_const :LocalJumpError
      g.push_literal "next used in invalid context"
      g.send :raise, 2, true
    end
  end
end
