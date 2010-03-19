require File.dirname(__FILE__) + '/../spec_helper'

describe "A Defined node" do
  relates "defined? $x" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      g.push_const :Rubinius
      g.find_const :Globals
      g.push_literal :$x
      g.send :key?, 1
      g.gif f
      g.push_literal "global-variable"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? a" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      g.push :self
      g.push_literal :a
      g.push :true
      g.send :__respond_to_eh__, 2
      g.gif f
      g.push_literal "method"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates <<-ruby do
      a = 1
      defined? a
    ruby

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop

      g.push_literal "local-variable"
    end
  end

  relates "defined? x = 1" do
    compile do |g|
      g.push_literal "assignment"
    end
  end

  relates "defined? x += 1" do
    compile do |g|
      g.push_literal "assignment"
    end
  end

  relates "defined? x ||= 1" do
    compile do |g|
      g.push_literal "assignment"
    end
  end

  relates "defined? x &&= 1" do
    compile do |g|
      g.push_literal "assignment"
    end
  end

  relates "defined? X" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      ex = g.new_label
      ok = g.new_label
      g.setup_unwind ex

      g.push_literal :X
      g.invoke_primitive :vm_const_defined, 1
      g.pop_unwind
      g.goto ok

      ex.set!
      g.clear_exception
      g.goto f

      ok.set!
      g.pop
      g.push_literal "constant"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? ::X" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      ex = g.new_label
      ok = g.new_label
      g.setup_unwind ex

      g.push_cpath_top
      g.push_literal :X
      g.push :false
      g.invoke_primitive :vm_const_defined_under, 3
      g.pop_unwind
      g.goto ok

      ex.set!
      g.clear_exception
      g.goto f

      ok.set!
      g.pop
      g.push_literal "constant"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? X::Y" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      ex = g.new_label
      ok = g.new_label
      g.setup_unwind ex

      g.push_const :X
      g.push_literal :Y
      g.push :false
      g.invoke_primitive :vm_const_defined_under, 3
      g.pop_unwind
      g.goto ok

      ex.set!
      g.clear_exception
      g.goto f

      ok.set!
      g.pop
      g.push_literal "constant"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? X::Y::Z" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      ex = g.new_label
      ok = g.new_label
      g.setup_unwind ex

      g.push_const :X
      g.find_const :Y
      g.push_literal :Z
      g.push :false
      g.invoke_primitive :vm_const_defined_under, 3
      g.pop_unwind
      g.goto ok

      ex.set!
      g.clear_exception
      g.goto f

      ok.set!
      g.pop
      g.push_literal "constant"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? self::A" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      ex = g.new_label
      ok = g.new_label
      g.setup_unwind ex

      g.push :self
      g.push_literal :A
      g.push :false
      g.invoke_primitive :vm_const_defined_under, 3
      g.pop_unwind
      g.goto ok

      ex.set!
      g.clear_exception
      g.goto f

      ok.set!
      g.pop
      g.push_literal "constant"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? self" do
    compile do |g|
      g.push_literal "self"
    end
  end

  relates "defined? true" do
    compile do |g|
      g.push_literal "true"
    end
  end

  relates "defined? false" do
    compile do |g|
      g.push_literal "false"
    end
  end

  relates "defined? nil" do
    compile do |g|
      g.push_literal "nil"
    end
  end

  relates "defined? @var" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      g.push :self
      g.push_literal :@var
      g.send :__instance_variable_defined_eh__, 1
      g.gif f
      g.push_literal "instance-variable"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? @@var" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      g.push_scope
      g.push_literal :@@var
      g.send :class_variable_defined?, 1
      g.gif f
      g.push_literal "class variable"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? :a" do
    compile do |g|
      g.push_literal "expression"
    end
  end

  relates "defined? 'a'" do
    compile do |g|
      g.push_literal "expression"
    end
  end

  relates "defined? 0" do
    compile do |g|
      g.push_literal "expression"
    end
  end

  relates "defined? yield" do
    compile do |g|
      t = g.new_label
      f = g.new_label

      g.push_block
      g.git t
      g.push :nil
      g.goto f

      t.set!
      g.push_literal "yield"

      f.set!
    end
  end

  relates "defined? A.m" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      ex = g.new_label
      ok = g.new_label
      g.setup_unwind ex

      g.push_literal :A
      g.invoke_primitive :vm_const_defined, 1

      g.pop_unwind
      g.goto ok

      ex.set!
      g.clear_exception
      g.goto f

      ok.set!
      g.push_literal :m
      g.push :false
      g.send :__respond_to_eh__, 2
      g.gif f
      g.push_literal "method"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? ::A.m" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      ex = g.new_label
      ok = g.new_label
      g.setup_unwind ex

      g.push_cpath_top
      g.push_literal :A
      g.push :false
      g.invoke_primitive :vm_const_defined_under, 3

      g.pop_unwind
      g.goto ok

      ex.set!
      g.clear_exception
      g.goto f

      ok.set!
      g.push_literal :m
      g.push :false
      g.send :__respond_to_eh__, 2
      g.gif f
      g.push_literal "method"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? A::B.m" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      ex = g.new_label
      ok = g.new_label
      g.setup_unwind ex

      g.push_const :A
      g.push_literal :B
      g.push :true
      g.invoke_primitive :vm_const_defined_under, 3

      g.pop_unwind
      g.goto ok

      ex.set!
      g.clear_exception
      g.goto f

      ok.set!
      g.push_literal :m
      g.push :false
      g.send :__respond_to_eh__, 2
      g.gif f
      g.push_literal "method"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates "defined? a.b" do
    compile do |g|
      f = g.new_label
      done = g.new_label

      ex = g.new_label
      ok = g.new_label
      g.setup_unwind ex

      g.push :self
      g.send :a, 0, true

      g.pop_unwind
      g.goto ok

      ex.set!
      g.clear_exception
      g.goto f

      ok.set!
      g.push_literal :b
      g.push :false
      g.send :__respond_to_eh__, 2
      g.gif f
      g.push_literal "method"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end

  relates <<-ruby do
      a = 1
      defined? a.to_s
    ruby

    compile do |g|
      f = g.new_label
      done = g.new_label

      g.push 1
      g.set_local 0
      g.pop

      g.push_local 0
      g.push_literal :to_s
      g.push :false
      g.send :__respond_to_eh__, 2
      g.gif f
      g.push_literal "method"
      g.goto done

      f.set!
      g.push :nil

      done.set!
    end
  end
end
