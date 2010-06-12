require File.expand_path('../../spec_helper', __FILE__)

describe "A Case node" do
  relates <<-ruby do
      var = 2
      result = ""
      case var
      when 1 then
        puts("something")
        result = "red"
      when 2, 3 then
        result = "yellow"
      when 4 then
        # do nothing
      else
        result = "green"
      end
      case result
      when "red" then
        var = 1
      when "yellow" then
        var = 2
      when "green" then
        var = 3
      else
        # do nothing
      end
    ruby

    compile do |g|
      a1 = g.new_label
      a2 = g.new_label
      a3 = g.new_label
      a4 = g.new_label
      a_bottom = g.new_label

      g.push 2
      g.set_local 0
      g.pop
      g.push_literal ""
      g.string_dup
      g.set_local 1
      g.pop

      g.push_local 0
      g.dup
      g.push 1
      g.swap
      g.send :===, 1
      g.gif a1

      g.pop
      g.push :self
      g.push_literal "something"
      g.string_dup
      g.send :puts, 1, true
      g.pop
      g.push_literal "red"
      g.string_dup
      g.set_local 1
      g.goto a_bottom

      a1.set!

      g.dup
      g.push 2
      g.swap
      g.send :===, 1
      g.git a2

      g.dup
      g.push 3
      g.swap
      g.send :===, 1
      g.git a2
      g.goto a3

      a2.set!

      g.pop
      g.push_literal "yellow"
      g.string_dup
      g.set_local 1
      g.goto a_bottom

      a3.set!

      g.dup
      g.push 4
      g.swap
      g.send :===, 1
      g.gif a4

      g.pop
      g.push :nil
      g.goto a_bottom

      a4.set!

      g.pop
      g.push_literal "green"
      g.string_dup
      g.set_local 1

      a_bottom.set!

      b1 = g.new_label
      b2 = g.new_label
      b3 = g.new_label
      b_bottom = g.new_label

      g.pop
      g.push_local 1
      g.dup
      g.push_literal "red"
      g.string_dup
      g.swap
      g.send :===, 1
      g.gif b1

      g.pop
      g.push 1
      g.set_local 0
      g.goto b_bottom

      b1.set!

      g.dup
      g.push_literal "yellow"
      g.string_dup
      g.swap
      g.send :===, 1
      g.gif b2

      g.pop
      g.push 2
      g.set_local 0
      g.goto b_bottom

      b2.set!

      g.dup
      g.push_literal "green"
      g.string_dup
      g.swap
      g.send :===, 1
      g.gif b3

      g.pop
      g.push 3
      g.set_local 0
      g.goto b_bottom

      b3.set!

      g.pop
      g.push :nil

      b_bottom.set!
    end
  end

  relates <<-ruby do
      case a
      when b then
        case
        when (d and e) then
          f
        else
          # do nothing
        end
      else
        # do nothing
      end
    ruby

    compile do |g|
      c2, bottom = g.new_label, g.new_label
      i1, i2, ibottom = g.new_label, g.new_label, g.new_label

      g.push :self
      g.send :a, 0, true
      g.dup
      g.push :self
      g.send :b, 0, true
      g.swap
      g.send :===, 1
      g.gif c2

      g.pop
      g.push :self
      g.send :d, 0, true
      g.dup
      g.gif i1          # TODO: lamest jump ever - should be ibottom

      g.pop
      g.push :self
      g.send :e, 0, true

      i1.set!

      g.gif i2
      g.push :self
      g.send :f, 0, true

      g.goto ibottom

      i2.set!

      g.push :nil

      ibottom.set!

      g.goto bottom

      c2.set!
      g.pop
      g.push :nil

      bottom.set!
    end
  end

  relates <<-ruby do
      var1 = 1
      var2 = 2
      result = nil
      case var1
      when 1 then
        case var2
        when 1 then
          result = 1
        when 2 then
          result = 2
        else
          result = 3
        end
      when 2 then
        case var2
        when 1 then
          result = 4
        when 2 then
          result = 5
        else
          result = 6
        end
      else
        result = 7
      end
    ruby

    compile do |g|
      a2 = g.new_label
      a3 = g.new_label
      a_bottom = g.new_label

      g.push 1
      g.set_local 0
      g.pop
      g.push 2
      g.set_local 1
      g.pop
      g.push :nil
      g.set_local 2
      g.pop

      ########################################
      b2 = g.new_label
      b3 = g.new_label
      b_bottom = g.new_label

      g.push_local 0
      g.dup
      g.push 1
      g.swap
      g.send :===, 1
      g.gif a2

      g.pop
      g.push_local 1
      g.dup
      g.push 1
      g.swap
      g.send :===, 1
      g.gif b2

      g.pop
      g.push 1
      g.set_local 2
      g.goto b_bottom

      b2.set!

      g.dup
      g.push 2
      g.swap
      g.send :===, 1
      g.gif b3

      g.pop
      g.push 2
      g.set_local 2
      g.goto b_bottom

      b3.set!

      g.pop
      g.push 3
      g.set_local 2

      b_bottom.set!

      g.goto a_bottom

      a2.set!

      g.dup
      g.push 2
      g.swap
      g.send :===, 1
      g.gif a3

      ########################################
      c2 = g.new_label
      c3 = g.new_label
      c_bottom = g.new_label

      g.pop
      g.push_local 1
      g.dup
      g.push 1
      g.swap
      g.send :===, 1
      g.gif c2

      g.pop
      g.push 4
      g.set_local 2
      g.goto c_bottom

      c2.set!

      g.dup
      g.push 2
      g.swap
      g.send :===, 1
      g.gif c3

      g.pop
      g.push 5
      g.set_local 2
      g.goto c_bottom

      c3.set!

      g.pop
      g.push 6
      g.set_local 2

      c_bottom.set!

      g.goto a_bottom

      a3.set!

      g.pop
      g.push 7
      g.set_local 2

      a_bottom.set!
    end
  end

  relates <<-ruby do
      case
      when (a == 1) then
        :a
      when (a == 2) then
        :b
      else
        :c
      end
    ruby

    compile do |g|
      c2, c3, bottom = g.new_label, g.new_label, g.new_label

      g.push :self
      g.send :a, 0, true
      g.push 1
      g.send :==, 1, false
      g.gif c2
      g.push_literal :a
      g.goto bottom

      c2.set!

      g.push :self
      g.send :a, 0, true
      g.push 2
      g.send :==, 1, false
      g.gif c3
      g.push_literal :b
      g.goto bottom

      c3.set!
      g.push_literal :c

      bottom.set!
    end
  end

  relates <<-ruby do
      case a
      when :b, *c then
        d
      else
        e
      end
    ruby

    compile do |g|
      c1, c2, bottom = g.new_label, g.new_label, g.new_label

      g.push :self
      g.send :a, 0, true

      g.dup
      g.push_literal :b
      g.swap
      g.send :===, 1
      g.git c1

      g.dup
      g.push :self
      g.send :c, 0, true
      g.cast_array
      g.swap
      g.send :__matches_when__, 1
      g.git c1

      g.goto c2

      c1.set!

      g.pop
      g.push :self
      g.send :d, 0, true
      g.goto bottom

      c2.set!

      g.pop
      g.push :self
      g.send :e, 0, true

      bottom.set!
    end
  end

  relates <<-ruby do
      case true
      when String, *%w(foo bar baz) then
        12
      end
    ruby

    compile do |g|
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

  relates <<-ruby do
      case ()
      when a
        1
      end
    ruby

    compile do |g|
      no_match = g.new_label
      done     = g.new_label

      g.push :nil
      g.dup
      g.push :self
      g.send :a, 0, true
      g.swap
      g.send :===, 1
      g.gif no_match

      g.pop
      g.push 1
      g.goto done

      no_match.set!
      g.pop
      g.push :nil
      done.set!
    end
  end

  relates <<-ruby do
      x = 1
      case a
      when x
        2
      end
    ruby

    compile do |g|
      no_match = g.new_label
      done     = g.new_label

      g.push 1
      g.set_local 0
      g.pop
      g.push :self
      g.send :a, 0, true
      g.dup
      g.push_local 0
      g.swap
      g.send :===, 1
      g.gif no_match
      g.pop
      g.push 2
      g.goto done
      no_match.set!
      g.pop
      g.push :nil
      done.set!
    end
  end
end
