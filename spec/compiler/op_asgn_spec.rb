require File.dirname(__FILE__) + '/../spec_helper'

describe "An Op_asgn1 node" do
  relates "a &&= 8" do
    compile do |g|
      fin = g.new_label

      g.push_local 0
      g.dup
      g.gif fin

      g.pop
      g.push 8
      g.set_local 0

      fin.set!
    end
  end

  relates "hsh[:blah] ||= 8" do
    compile do |g|
      found = g.new_label
      fin = g.new_label

      g.push :self
      g.send :hsh, 0, true
      g.push_literal :blah
      g.dup_many 2

      g.send :[], 1
      g.dup
      g.git found

      g.pop
      g.push 8
      g.dup
      g.move_down 3
      g.send :[]=, 2
      g.pop
      g.goto fin

      found.set!

      g.move_down 2
      g.pop_many 2

      fin.set!
    end
  end

  relates <<-ruby do
      x = 1
      hsh[x] ||= 8
    ruby

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop

      found = g.new_label
      fin = g.new_label

      g.push :self
      g.send :hsh, 0, true
      g.push_local 0
      g.dup_many 2

      g.send :[], 1
      g.dup
      g.git found

      g.pop
      g.push 8
      g.dup
      g.move_down 3
      g.send :[]=, 2
      g.pop
      g.goto fin

      found.set!

      g.move_down 2
      g.pop_many 2

      fin.set!
    end
  end

  relates "hsh[:blah] &&= 8" do
    compile do |g|
      found = g.new_label
      fin = g.new_label

      g.push :self
      g.send :hsh, 0, true
      g.push_literal :blah
      g.dup_many 2

      g.send :[], 1
      g.dup
      g.gif found

      g.pop
      g.push 8
      g.dup
      g.move_down 3
      g.send :[]=, 2
      g.pop
      g.goto fin

      found.set!

      g.move_down 2
      g.pop_many 2

      fin.set!
    end

  end

  relates "hsh[:blah] ^= 8" do
    compile do |g|
      fin = g.new_label

      g.push :self
      g.send :hsh, 0, true
      g.push_literal :blah
      g.dup_many 2

      g.send :[], 1
      g.push 8
      g.send :"^", 1
      g.dup
      g.move_down 3
      g.send :[]=, 2
      g.pop
    end
  end

  relates "ary[0,1] += [4]" do
    compile do |g|
      g.push :self
      g.send :ary, 0, true
      g.push 0
      g.push 1
      g.dup_many 3

      g.send :[], 2

      g.push 4
      g.make_array 1
      g.send :"+", 1
      g.dup
      g.move_down 4
      g.send :[]=, 3
      g.pop
    end
  end

  relates "x.val ||= 6" do
    compile do |g|
      fnd = g.new_label
      fin = g.new_label

      g.push :self
      g.send :x, 0, true
      g.dup
      g.send :val, 0
      g.dup
      g.git fnd

      g.pop
      g.push 6
      g.dup
      g.move_down 2
      g.send :val=, 1
      g.pop
      g.goto fin

      fnd.set!
      g.swap
      g.pop
      fin.set!
    end
  end

  relates "x.val &&= 7" do
    compile do |g|
      fnd = g.new_label
      fin = g.new_label

      g.push :self
      g.send :x, 0, true
      g.dup
      g.send :val, 0
      g.dup
      g.gif fnd

      g.pop
      g.push 7
      g.dup
      g.move_down 2
      g.send :val=, 1
      g.pop
      g.goto fin

      fnd.set!
      g.swap
      g.pop
      fin.set!
    end
  end

end

describe "An Op_asgn2 node" do
  relates "x.val ^= 8" do
    compile do |g|
      g.push :self
      g.send :x, 0, true
      g.dup
      g.send :val, 0
      g.push 8
      g.send :"^", 1
      g.dup
      g.move_down 2
      g.send :val=, 1
      g.pop
    end
  end

  relates "self.Bag ||= Bag.new" do
    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push :self
      g.dup
      g.send :Bag, 0
      g.dup
      g.git t
      g.pop
      g.push_const :Bag
      g.send :new, 0, false
      g.dup
      g.move_down 2
      g.send :"Bag=", 1
      g.pop
      g.goto f

      t.set!

      g.swap
      g.pop

      f.set!
    end
  end

  relates <<-ruby do
      s = Struct.new(:var)
      c = s.new(nil)
      c.var ||= 20
      c.var &&= 21
      c.var += 22
      c.d.e.f ||= 42
    ruby

    compile do |g|
      l_or = g.new_label
      l_and = g.new_label
      l_plus = g.new_label
      l_or2 = g.new_label
      l_rhs = g.new_label
      bottom = g.new_label

      g.push_const :Struct
      g.push_literal :var
      g.send :new, 1, false
      g.set_local 0
      g.pop

      g.push_local 0
      g.push :nil
      g.send :new, 1, false
      g.set_local 1
      g.pop

      g.push_local 1
      g.dup
      g.send :var, 0
      g.dup
      g.git l_or

      g.pop
      g.push 20
      g.dup
      g.move_down 2
      g.send :var=, 1
      g.pop
      g.goto l_and

      l_or.set!

      g.swap
      g.pop

      l_and.set!

      g.pop
      g.push_local 1
      g.dup
      g.send :var, 0
      g.dup
      g.gif l_plus
      g.pop
      g.push 21
      g.dup
      g.move_down 2
      g.send :var=, 1
      g.pop
      g.goto l_or2

      l_plus.set!

      g.swap
      g.pop

      l_or2.set!

      g.pop
      g.push_local 1
      g.dup
      g.send :var, 0
      g.push 22
      g.send :+, 1
      g.dup
      g.move_down 2
      g.send :var=, 1
      g.pop
      g.pop

      g.push_local 1
      g.send :d, 0, false
      g.send :e, 0, false
      g.dup
      g.send :f, 0
      g.dup

      g.git l_rhs

      g.pop
      g.push 42
      g.dup
      g.move_down 2
      g.send :f=, 1
      g.pop
      g.goto bottom

      l_rhs.set!

      g.swap
      g.pop

      bottom.set!
    end
  end
end

describe "An Op_asgn_and node" do
  relates "@fetcher &&= new(Gem.configuration[:http_proxy])" do
    compile do |g|
      t = g.new_label

      g.push_ivar :@fetcher
      g.dup
      g.gif t
      g.pop

      g.push :self
      g.push_const :Gem
      g.send :configuration, 0, false

      g.push_literal :http_proxy
      g.send :[], 1, false

      g.send :new, 1, true

      g.set_ivar :@fetcher

      t.set!
    end
  end

  relates <<-ruby do
      a = 0
      a &&= 2
    ruby

    compile do |g|
      g.push 0
      g.set_local 0
      g.pop

      g.push_local 0
      g.dup

      f = g.new_label
      g.gif f
      g.pop
      g.push 2
      g.set_local 0

      f.set!
    end
  end
end

describe "An Op_asgn_or node" do
  relates "@@var ||= 3" do
    compile do |g|
      done = g.new_label
      notfound = g.new_label

      g.push_scope
      g.push_literal :@@var
      g.send :class_variable_defined?, 1
      g.gif notfound

      g.push_scope
      g.push_literal :@@var
      g.send :class_variable_get, 1
      g.dup
      g.git done
      g.pop

      notfound.set!
      g.push_scope
      g.push_literal :@@var
      g.push 3
      g.send :class_variable_set, 2

      done.set!
    end
  end

  relates "a ||= 8" do
    compile do |g|
      fin = g.new_label

      g.push_local 0
      g.dup
      g.git fin

      g.pop
      g.push 8
      g.set_local 0

      fin.set!
    end
  end

  relates <<-ruby do
      a ||= begin
              b
            rescue
              c
            end
    ruby

    compile do |g|
      t = g.new_label

      g.push_local 0
      g.dup
      g.git t
      g.pop

      for_rescue do |rb|
        rb.body do
          g.push :self
          g.send :b, 0, true
        end

        rb.condition :StandardError do
          g.push :self
          g.send :c, 0, true
        end
      end

      g.set_local 0

      t.set!
    end
  end

  relates "@fetcher ||= new(Gem.configuration[:http_proxy])" do
    compile do |g|
      t = g.new_label

      g.push_ivar :@fetcher
      g.dup
      g.git t
      g.pop

      g.push :self
      g.push_const :Gem
      g.send :configuration, 0, false

      g.push_literal :http_proxy
      g.send :[], 1, false

      g.send :new, 1, true

      g.set_ivar :@fetcher

      t.set!
    end
  end

  relates "@v ||= {  }" do
    compile do |g|
      t = g.new_label

      g.push_ivar :@v
      g.dup
      g.git t
      g.pop

      g.push_cpath_top
      g.find_const :Hash
      g.push 0
      g.send :new_from_literal, 1

      g.set_ivar :@v

      t.set!
    end
  end

  relates <<-ruby do
      a = 0
      a ||= 1
    ruby

    compile do |g|
      t = g.new_label

      g.push 0
      g.set_local 0
      g.pop             # FIX: lame
      g.push_local 0
      g.dup
      g.git t
      g.pop

      g.push 1

      g.set_local 0

      t.set!
    end
  end
end
