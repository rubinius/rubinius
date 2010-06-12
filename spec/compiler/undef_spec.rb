require File.expand_path('../../spec_helper', __FILE__)

describe "An Undef node" do
  relates "undef :x" do
    compile do |g|
      undef_bytecode :x
    end
  end

  relates "undef :x, :y" do
    compile do |g|
      undef_bytecode :x, :y
    end
  end

  relates "undef :x, :y, :z" do
    compile do |g|
      undef_bytecode :x, :y, :z
    end
  end

  relates <<-ruby do
      f1
      undef :x
    ruby

    compile do |g|
      g.push :self
      g.send :f1, 0, true
      g.pop

      undef_bytecode :x
    end
  end

  relates <<-ruby do
      f1
      undef :x, :y
    ruby

    compile do |g|
      g.push :self
      g.send :f1, 0, true
      g.pop

      undef_bytecode :x, :y
    end
  end

  relates <<-ruby do
      undef :x, :y, :z
      f2
    ruby

    compile do |g|
      undef_bytecode :x, :y, :z
      g.pop

      g.push :self
      g.send :f2, 0, true
    end
  end

  relates <<-ruby do
      f1
      undef :x, :y, :z
    ruby

    compile do |g|
      g.push :self
      g.send :f1, 0, true
      g.pop

      undef_bytecode :x, :y, :z
    end
  end

  relates <<-ruby do
      f1
      undef :x, :y, :z
      f2
    ruby

    compile do |g|
      g.push :self
      g.send :f1, 0, true
      g.pop

      undef_bytecode :x, :y, :z
      g.pop

      g.push :self
      g.send :f2, 0, true
    end
  end

  relates "class B; undef :blah; end" do
    compile do |g|
      g.in_class :B do |d|
        d.push_scope
        d.push_literal :blah
        d.send :__undef_method__, 1
      end
    end
  end

  relates <<-ruby do
      undef :"x\#{1}", :"x\#{2}"
    ruby

    compile do |g|
      g.push_scope
      g.push_literal "x"

      g.push 1
      g.send :to_s, 0, true

      g.string_build 2

      g.send :to_sym, 0, true
      g.send :__undef_method__, 1
      g.pop

      g.push_scope

      g.push_literal "x"
      g.push 2
      g.send :to_s, 0, true
      g.string_build 2

      g.send :to_sym, 0, true
      g.send :__undef_method__, 1
    end
  end
end
