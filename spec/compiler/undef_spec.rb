require File.dirname(__FILE__) + '/../spec_helper'

describe "An Undef node" do
  relates "undef :x" do
    parse do
      [:undef, [:lit, :x]]
    end

    compile do |g|
      undef_bytecode :x
    end
  end

  relates "undef :x, :y" do
    parse do
      [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]]]
    end

    compile do |g|
      undef_bytecode :x, :y
    end
  end

  relates "undef :x, :y, :z" do
    parse do
      [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]], [:undef, [:lit, :z]]]
    end

    compile do |g|
      undef_bytecode :x, :y, :z
    end
  end

  relates <<-ruby do
      f1
      undef :x
    ruby

    parse do
      [:block, [:call, nil, :f1, [:arglist]], [:undef, [:lit, :x]]]
    end

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

    parse do
      [:block,
       [:call, nil, :f1, [:arglist]],
       [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]]]]
    end

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

    parse do
      [:block,
       [:undef, [:lit, :x]],
       [:undef, [:lit, :y]],
       [:undef, [:lit, :z]],
       [:call, nil, :f2, [:arglist]]]
    end

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

    parse do
      [:block,
       [:call, nil, :f1, [:arglist]],
       [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]], [:undef, [:lit, :z]]]]
    end

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

    parse do
      [:block,
       [:call, nil, :f1, [:arglist]],
       [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]], [:undef, [:lit, :z]]],
       [:call, nil, :f2, [:arglist]]]
    end

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
    parse do
      [:class, :B, nil, [:scope, [:undef, [:lit, :blah]]]]
    end

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

    parse do
      [:block,
        [:undef, [:dsym, "x", [:evstr, [:lit, 1]]]],
        [:undef, [:dsym, "x", [:evstr, [:lit, 2]]]]]
    end

    compile do |g|
      g.push_scope
      g.push 1
      g.send :to_s, 0, true
      g.push_literal "x"
      g.string_dup
      g.string_append
      g.send :to_sym, 0, true
      g.send :__undef_method__, 1
      g.pop

      g.push_scope
      g.push 2
      g.send :to_s, 0, true
      g.push_literal "x"
      g.string_dup
      g.string_append
      g.send :to_sym, 0, true
      g.send :__undef_method__, 1
    end
  end
end
