require File.dirname(__FILE__) + '/../spec_helper'

describe "An Lasgn node" do
  relates "a = 1" do
    parse do
      [:lasgn, :a, [:lit, 1]]
    end

    compile do |g|
      g.push 1
      g.set_local 0
    end
  end

  relates "a = b, c, *d" do
    parse do
      [:lasgn,
         :a,
         [:svalue,
          [:array,
           [:call, nil, :b, [:arglist]],
           [:call, nil, :c, [:arglist]],
           [:splat, [:call, nil, :d, [:arglist]]]]]]
    end

    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.push :self
      g.send :c, 0, true
      g.make_array 2
      g.push :self
      g.send :d, 0, true
      g.cast_array
      g.send :+, 1
      g.set_local 0
    end
  end

  relates "a = [b, *c]" do
    parse do
      [:lasgn,
         :a,
         [:array,
          [:call, nil, :b, [:arglist]],
          [:splat, [:call, nil, :c, [:arglist]]]]]
    end

    compile do |g|
      g.push :self
      g.send :b, 0, true
      g.make_array 1
      g.push :self
      g.send :c, 0, true
      g.cast_array
      g.send :+, 1
      g.set_local 0
    end
  end

  relates <<-ruby do
      x = (y = 1
           (y + 2))
    ruby

    parse do
      [:lasgn,
         :x,
         [:block,
          [:lasgn, :y, [:lit, 1]],
          [:call, [:lvar, :y], :+, [:arglist, [:lit, 2]]]]]
    end

    compile do |g|
      g.push 1
      g.set_local 1
      g.pop
      g.push_local 1
      g.push 2
      g.send :+, 1, false
      g.set_local 0
    end
  end

  relates <<-ruby do
      a = []
      a [42]
    ruby

    parse do
      [:block,
       [:lasgn, :a, [:array]],
       [:call, [:lvar, :a], :[], [:arglist, [:lit, 42]]]]
    end

    # call index space
  end

  relates 'var = ["foo", "bar"]' do
    parse do
      [:lasgn, :var, [:array, [:str, "foo"], [:str, "bar"]]]
    end

    compile do |g|
      g.push_literal "foo"
      g.string_dup
      g.push_literal "bar"
      g.string_dup
      g.make_array 2
      g.set_local 0
    end
  end

  relates "c = (2 + 3)" do
    parse do
      [:lasgn, :c, [:call, [:lit, 2], :+, [:arglist, [:lit, 3]]]]
    end

    compile do |g|
      g.push 2
      g.push 3
      g.send :+, 1, false
      g.set_local 0
    end
  end

  relates "a = *[1]" do
    parse do
      [:lasgn, :a, [:svalue, [:splat, [:array, [:lit, 1]]]]]
    end

    compile do |g|
      g.splatted_array
      g.set_local 0
    end
  end

  relates "a = *b" do
    parse do
      [:lasgn, :a, [:svalue, [:splat, [:call, nil, :b, [:arglist]]]]]
    end

    compile do |g|
      t = g.new_label

      g.push :self
      g.send :b, 0, true
      g.cast_array
      g.dup

      g.send :size, 0
      g.push 1
      g.send :>, 1
      g.git t

      g.push 0
      g.send :at, 1
      t.set!
      g.set_local 0
    end
  end

  relates <<-ruby do
      a = if c
            begin
              b
            rescue
              nil
            end
          end
      a
    ruby

    parse do
      [:block,
       [:lasgn,
        :a,
        [:if,
         [:call, nil, :c, [:arglist]],
         [:rescue, [:call, nil, :b, [:arglist]], [:resbody, [:array], [:nil]]],
         nil]],
       [:lvar, :a]]
    end

    compile do |g|
      bottom = g.new_label
      f      = g.new_label

      g.push :self
      g.send :c, 0, true
      g.gif f

      in_rescue :StandardError, 1 do |section|
        case section
        when :body then
          g.push :self
          g.send :b, 0, true
        when :StandardError then
          g.push :nil
        end
      end
      g.goto bottom

      f.set!
      g.push :nil

      bottom.set!
      g.set_local 0
      g.pop
      g.push_local 0
    end
  end

  relates "x = [*[1]]" do
    parse do
      [:lasgn, :x, [:array, [:splat, [:array, [:lit, 1]]]]]
    end

    compile do |g|
      g.array_of_splatted_array
      g.set_local 0
    end
  end

  relates "a = []" do
    parse do
      [:lasgn, :a, [:array]]
    end

    compile do |g|
      g.make_array 0
      g.set_local 0
    end
  end

  relates <<-ruby do
      a = 12
      a
    ruby

    parse do
      [:block, [:lasgn, :a, [:lit, 12]], [:lvar, :a]]
    end

    compile do |g|
      g.push 12
      g.set_local 0
      g.pop
      g.push_local 0
    end
  end

  relates <<-ruby do
      name
      name = 3
      name
    ruby

    parse do
      [:block,
       [:call, nil, :name, [:arglist]],
       [:lasgn, :name, [:lit, 3]],
       [:lvar, :name]]
    end
  end

  relates "a=12; b=13; true" do
    parse do
      [:block, [:lasgn, :a, [:lit, 12]], [:lasgn, :b, [:lit, 13]], [:true]]
    end

    compile do |g|
      g.push 12
      g.set_local 0
      g.pop
      g.push 13
      g.set_local 1
      g.pop
      g.push :true
    end
  end
end
