require File.expand_path('../../spec_helper', __FILE__)

describe "An Iter node" do
  empty_block = lambda do |g|
    g.push :self

    g.in_block_send :m, :none do |d|
      d.push :nil
    end
  end

  relates "m { }" do
    compile(&empty_block)
  end

  relates "m do end" do
    compile(&empty_block)
  end

  relates "m { x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push :self
        d.send :x, 0, true
      end
    end
  end

  relates "m { || x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :empty do |d|
        d.push :self
        d.send :x, 0, true
      end
    end
  end

  single_arg_block = lambda do |g|
    g.push :self

    g.in_block_send :m, :single do |d|
      d.push_local 0
      d.push :self
      d.send :x, 0, true
      d.send :+, 1, false
    end
  end

  relates "m { |a| a + x }" do
    compile(&single_arg_block)
  end

  relates "m { |*| x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :blank do |d|
        d.push :self
        d.send :x, 0, true
      end
    end
  end

  relates "m { |*c| x; c }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :splat do |d|
        d.push :self
        d.send :x, 0, true
        d.pop
        d.push_local 0
      end
    end
  end

  masgn_single_arg_block = lambda do |g|
    g.push :self

    g.in_block_send :m, :multi, 1 do |d|
      d.push_local 0
      d.push :self
      d.send :x, 0, true
      d.send :+, 1, false
    end
  end

  relates "m { |a, | a + x }" do
    compile(&masgn_single_arg_block)
  end

  relates "m { |a, *| a + x }" do
    compile(&masgn_single_arg_block)
  end

  relates "m { |a, *c| a + x; c }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :rest, -2 do |d|
        d.push_local 0
        d.push :self
        d.send :x, 0, true
        d.send :+, 1, false
        d.pop
        d.push_local 1
      end
    end
  end

  relates "m { |a, b| a + x; b }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :multi, 2 do |d|
        d.push_local 0
        d.push :self
        d.send :x, 0, true
        d.send :+, 1, false
        d.pop
        d.push_local 1
      end
    end
  end

  masgn_multi_arg_block = lambda do |g|
    g.push :self

    g.in_block_send :m, :multi, -2 do |d|
      d.push_local 0
      d.push :self
      d.send :x, 0, true
      d.send :+, 1, false
      d.pop
      d.push_local 1
    end
  end

  relates "m { |a, b, | a + x; b }" do
    compile(&masgn_multi_arg_block)
  end

  relates "m { |a, b, *| a + x; b }" do
    compile(&masgn_multi_arg_block)
  end

  masgn_rest_arg_block = lambda do |g|
    g.push :self

    g.in_block_send :m, :rest, -3 do |d|
      d.push_local 0
      d.push :self
      d.send :x, 0, true
      d.send :+, 1, false
      d.pop
      d.push_local 1
      d.pop
      d.push_local 2
    end
  end

  relates "m { |a, b, *c| a + x; b; c }" do
    compile(&masgn_rest_arg_block)
  end

  relates "m do |a, b, *c| a + x; b; c end" do
    compile(&masgn_rest_arg_block)
  end

  relates "m { n = 1; n }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.set_local 0
        d.pop
        d.push_local 0
      end
    end
  end

  relates "m { n = 1; m { n } }" do
    compile do |g|
      d = new_block_generator(g)

      d.push_modifiers
      d.new_label.set! # redo
      d.push 1
      d.set_local 0
      d.pop

      e = new_block_generator(g)

      e.push_modifiers
      e.new_label.set! # redo
      e.push_local_depth 1, 0
      e.pop_modifiers
      e.ret

      d.push :self
      d.create_block(e)
      d.send_with_block :m, 0, true

      d.pop_modifiers
      d.ret

      g.push :self
      g.create_block(d)
      g.send_with_block :m, 0, true
    end
  end

  relates "n = 1; m { n = 2 }; n" do
    compile do |g|
      g.push 1
      g.set_local 0
      g.pop

      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 2
        d.set_local_depth 1, 0
      end

      g.pop
      g.push_local 0
    end
  end

  relates "m(a) { |b| a + x }" do
    compile do |g|
      g.push :self
      g.push :self
      g.send :a, 0, true

      g.in_block_send :m, :single, nil, 1 do |d|
        d.push :self
        d.send :a, 0, true
        d.push :self
        d.send :x, 0, true
        d.send :+, 1, false
      end
    end
  end

  relates <<-ruby do
      m { |a|
        a + x
      }
    ruby

    compile(&single_arg_block)
  end

  relates <<-ruby do
      m do |a|
        a + x
      end
    ruby

    compile(&single_arg_block)
  end

  relates "obj.m { |a| a + x }" do
    compile do |g|
      g.push :self
      g.send :obj, 0, true

      g.in_block_send :m, :single, nil, 0, false do |d|
        d.push_local 0
        d.push :self
        d.send :x, 0, true
        d.send :+, 1, false
      end
    end
  end

  relates "obj.m(x) { |a| a + x }" do
    compile do |g|
      g.push :self
      g.send :obj, 0, true
      g.push :self
      g.send :x, 0, true

      g.in_block_send :m, :single, nil, 1, false do |d|
        d.push_local 0
        d.push :self
        d.send :x, 0, true
        d.send :+, 1, false
      end
    end
  end

  relates "obj.m(a) { |a| a + x }" do
    compile do |g|
      g.push :self
      g.send :obj, 0, true
      g.push :self
      g.send :a, 0, true

      g.in_block_send :m, :single, nil, 1, false do |d|
        d.push_local 0
        d.push :self
        d.send :x, 0, true
        d.send :+, 1, false
      end
    end
  end

  relates "a = 1; m { |a| a + x }" do
    compile do |g|
      g.push 1
      g.set_local 0
      g.pop

      g.push :self

      d = new_block_generator(g)

      d.cast_for_single_block_arg
      d.set_local_depth 1, 0

      d.pop
      d.push_modifiers
      d.new_label.set!

      d.push_local_depth 1, 0
      d.push :self
      d.send :x, 0, true
      d.send :+, 1, false

      d.pop_modifiers
      d.ret

      g.create_block(d)

      g.send_with_block :m, 0, true
    end
  end

  relates <<-ruby do
      x = nil
      m do |a|
        begin
          x
        rescue Exception => x
          break
        ensure
          x = a
        end
      end
    ruby

    compile do |g|
      g.push :nil
      g.set_local 0
      g.pop
      g.push :self

      d = new_block_generator(g)

      d.cast_for_single_block_arg
      d.set_local 0
      d.pop
      d.push_modifiers

      redo_lbl = d.new_label
      redo_lbl.set!

      d.for_ensure do |eb|
        eb.body do
          d.for_rescue do |rb|
            rb.body do
              d.push_local_depth 1, 0
            end

            rb.condition :Exception do
              d.push_current_exception
              d.set_local_depth 1, 0
              d.pop
              d.push :nil

              rb.break
            end
          end
        end

        eb.handler do
          d.push_local 0
          d.set_local_depth 1, 0
          d.pop
        end
      end

      d.pop_modifiers
      d.ret

      g.create_block(d)

      g.send_with_block :m, 0, true
    end
  end

  relates "m { next }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push :nil
        d.ret
      end
    end
  end

  relates "m { next if x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        f = d.new_label
        done = d.new_label

        d.push :self
        d.send :x, 0, true
        d.gif f

        d.push :nil
        d.ret
        d.goto done

        f.set!
        d.push :nil

        done.set!
      end
    end
  end

  relates "m { next x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push :self
        d.send :x, 0, true
        d.ret
      end
    end
  end

  relates "m { x = 1; next x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.set_local 0
        d.pop
        d.push_local 0
        d.ret
      end
    end
  end

  relates "m { next [1] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.make_array 1
        d.ret
      end
    end
  end

  relates "m { next *[1] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.splatted_array
        d.ret
      end
    end
  end

  relates "m { next [*[1]] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.make_array 1
        d.ret
      end
    end
  end

  relates "m { next *[1, 2] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.splatted_array 2 do
          d.push 1
          d.push 2
        end
        d.ret
      end
    end
  end

  relates "m { next [*[1, 2]] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.push 2
        d.make_array 2
        d.ret
      end
    end
  end

  relates "m { break }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push :nil
        d.raise_break
      end
    end
  end

  relates "m { break if x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        f = d.new_label
        done = d.new_label

        d.push :self
        d.send :x, 0, true
        d.gif f

        d.push :nil
        d.raise_break
        d.goto done

        f.set!
        d.push :nil

        done.set!
      end
    end
  end

  relates "m { break x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push :self
        d.send :x, 0, true
        d.raise_break
      end
    end
  end

  relates "m { x = 1; break x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.set_local 0
        d.pop
        d.push_local 0
        d.raise_break
      end
    end
  end

  relates "m { break [1] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.make_array 1
        d.raise_break
      end
    end
  end

  relates "m { break *[1] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.splatted_array
        d.raise_break
      end
    end
  end

  relates "m { break [*[1]] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.make_array 1
        d.raise_break
      end
    end
  end

  relates "m { break *[1, 2] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.splatted_array 2 do
          d.push 1
          d.push 2
        end
        d.raise_break
      end
    end
  end

  relates "m { break [*[1, 2]] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.push 2
        d.make_array 2
        d.raise_break
      end
    end
  end

  relates "m { return }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push :nil
        d.raise_return
      end
    end
  end

  relates "m { return if x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        f = d.new_label
        done = d.new_label

        d.push :self
        d.send :x, 0, true
        d.gif f

        d.push :nil
        d.raise_return
        d.goto done

        f.set!
        d.push :nil

        done.set!
      end
    end
  end

  relates "m { return x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push :self
        d.send :x, 0, true
        d.raise_return
      end
    end
  end

  relates "m { x = 1; return x }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.set_local 0
        d.pop
        d.push_local 0
        d.raise_return
      end
    end
  end

  relates "m { return [1] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.make_array 1
        d.raise_return
      end
    end
  end

  relates "m { return *[1] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.splatted_array
        d.raise_return
      end
    end
  end

  relates "m { return [*[1]] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.make_array 1
        d.raise_return
      end
    end
  end

  relates "m { return *[1, 2] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.splatted_array 2 do
          d.push 1
          d.push 2
        end
        d.raise_return
      end
    end
  end

  relates "m { return [*[1, 2]] }" do
    compile do |g|
      g.push :self

      g.in_block_send :m, :none do |d|
        d.push 1
        d.push 2
        d.make_array 2
        d.raise_return
      end
    end
  end

  relates "m { redo }" do
    compile do |g|
      g.push :self

      d = new_block_generator(g)

      redo_lbl = d.new_label
      d.push_modifiers
      redo_lbl.set!
      d.goto redo_lbl
      d.pop_modifiers
      d.ret

      g.create_block(d)

      g.send_with_block :m, 0, true
    end
  end

  relates "m { redo if x }" do
    compile do |g|
      g.push :self

      d = new_block_generator(g)

      redo_lbl = d.new_label
      f = d.new_label
      done = d.new_label

      d.push_modifiers
      redo_lbl.set!

      d.push :self
      d.send :x, 0, true
      d.gif f

      d.goto redo_lbl
      d.goto done

      f.set!
      d.push :nil

      done.set!
      d.pop_modifiers
      d.ret

      g.create_block(d)

      g.send_with_block :m, 0, true
    end
  end

  relates "m(a) { retry }" do
    # TODO
  end

  relates "m(a) { retry if x }" do
    # TODO
  end

  relates "break" do
    compile do |g|
      g.push :nil
      g.pop
      g.push_rubinius
      g.push_literal :break
      g.send :jump_error, 1
    end
  end

  relates "redo" do
    compile do |g|
      g.push_rubinius
      g.push_literal :redo
      g.send :jump_error, 1
    end
  end

  relates "retry" do
    compile do |g|
      g.push_rubinius
      g.push_literal :retry
      g.send :jump_error, 1
    end
  end

  relates "next" do
    compile do |g|
      g.push :nil
      g.pop
      g.push_rubinius
      g.push_literal :next
      g.send :jump_error, 1
    end
  end

  relates <<-ruby do
      def x(a)
        bar { super }
      end
    ruby

    compile do |g|
      in_method :x do |d|
        d.push :self

        d.in_block_send :bar, :none do |e|
          e.push_block
          e.zsuper :x
        end
      end
    end
  end
end
