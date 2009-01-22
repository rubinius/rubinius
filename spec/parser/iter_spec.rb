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

    # block_mystery
  end

  relates "loop { break 42 if true }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:if, [:true], [:break, [:lit, 42]], nil]]
    end

    # break arg
  end

  relates "loop { break if true }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:if, [:true], [:break], nil]]
    end

    # break
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

    # dasgn 0
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

    # dasgn 1
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

    # dasgn 2
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

    # dasgn curr
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

    # dasgn icky
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

    # dasgn mixed
  end

  relates "a (1) {|c|d}" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist, [:lit, 1]]],
       [:lasgn, :c],
       [:call, nil, :d, [:arglist]]]
    end

    # iter call arglist space
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

    # iter dasgn curr dasgn madness
  end

  relates "3.downto(1) { |n| puts(n.to_s) }" do
    parse do
      [:iter,
       [:call, [:lit, 3], :downto, [:arglist, [:lit, 1]]],
       [:lasgn, :n],
       [:call, nil, :puts, [:arglist, [:call, [:lvar, :n], :to_s, [:arglist]]]]]
    end

    # iter downto
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

    # iter each lvar
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

    # iter each nested
  end

  relates "loop { }" do
    parse do
      [:iter, [:call, nil, :loop, [:arglist]], nil]
    end

    # iter loop empty
  end

  relates "a { |b, c| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:lasgn, :b], [:lasgn, :c]]],
       [:call, nil, :p, [:arglist, [:lvar, :c]]]]
    end

    # iter masgn 2
  end

  relates "a { |b, c, *| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:lasgn, :b], [:lasgn, :c], [:splat]]],
       [:call, nil, :p, [:arglist, [:lvar, :c]]]]
    end

    # iter masgn args splat no name
  end

  relates "a { |b, c, *d| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:lasgn, :b], [:lasgn, :c], [:splat, [:lasgn, :d]]]],
       [:call, nil, :p, [:arglist, [:lvar, :c]]]]
    end

    # iter masgn args splat
  end

  relates "a { |*| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:splat]]],
       [:call, nil, :p, [:arglist, [:call, nil, :c, [:arglist]]]]]
    end

    # iter masgn splat no name
  end

  relates "a { |*c| p(c) }" do
    parse do
      [:iter,
       [:call, nil, :a, [:arglist]],
       [:masgn, [:array, [:splat, [:lasgn, :c]]]],
       [:call, nil, :p, [:arglist, [:lvar, :c]]]]
    end

    # iter masgn splat
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

    # iter shadowed var
  end

  relates "1.upto(3) { |n| puts(n.to_s) }" do
    parse do
      [:iter,
       [:call, [:lit, 1], :upto, [:arglist, [:lit, 3]]],
       [:lasgn, :n],
       [:call, nil, :puts, [:arglist, [:call, [:lvar, :n], :to_s, [:arglist]]]]]
    end

    # iter upto
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

    # iter while
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

    # structure extra block for dvar scoping
  end

  relates "loop { next if false }" do
    parse do
      [:iter,
        [:call, nil, :loop, [:arglist]], nil, [:if, [:false], [:next], nil]]
    end

    # next
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

    # splat next
  end

  relates "loop { next [*[1]] }" do
    parse do
      [:iter,
       [:call, nil, :loop, [:arglist]],
       nil,
       [:next, [:array, [:splat, [:array, [:lit, 1]]]]]]
    end

    # splat next array
  end

  relates "loop { redo if false }" do
    parse do
      [:iter,
        [:call, nil, :loop, [:arglist]], nil, [:if, [:false], [:redo], nil]]
    end

    # redo
  end

  relates "loop { retry }" do
    parse do
      [:iter, [:call, nil, :loop, [:arglist]], nil, [:retry]]
    end

    # retry
  end
end
