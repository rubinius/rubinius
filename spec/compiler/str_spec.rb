require File.dirname(__FILE__) + '/../spec_helper'

describe "A Str node" do
  relates '"x"' do
    parse do
      [:str, "x"]
    end

    compile do |g|
      g.push_literal "x"
      g.string_dup
    end
  end

  str_concat = lambda do |g|
    g.push_literal "before after"
    g.string_dup
  end

  relates <<-ruby do
      "before" \
      " after"
    ruby

    parse do
      [:str, "before after"]
    end

    compile(&str_concat)
  end

  relates '"before" " after"' do
    parse do
      [:str, "before after"]
    end

    compile(&str_concat)
  end

  relates <<-ruby do
      "file = \#{__FILE__}\n"
    ruby

    parse do
      [:dstr, "file = ", [:evstr, [:file]], [:str, "\n"]]
    end

    compile do |g|
      g.push_literal "\n"
      g.string_dup
      g.push_scope
      g.send :active_path, 0
      g.send :to_s, 0, true
      g.push_literal "file = "
      g.string_dup
      g.string_append
      g.string_append
    end
  end

  relates <<-ruby do
<<'EOM'.strip
  blah
blah
EOM
    ruby

    parse do
      [:call, [:str, "  blah\nblah\n"], :strip, [:arglist]]
    end

    compile do |g|
      g.push_literal "  blah\nblah\n"
      g.string_dup
      g.send :strip, 0, false
    end
  end

  relates <<-ruby do
a += <<-H1 + b + <<-H2
  first
H1
  second
H2
    ruby

    parse do
      [:lasgn,
       :a,
       [:call,
        [:lvar, :a],
        :+,
        [:arglist,
         [:call,
          [:call, [:str, "  first\n"], :+, [:arglist, [:call, nil, :b, [:arglist]]]],
          :+,
          [:arglist, [:str, "  second\n"]]]]]]
    end

    compile do |g|
      g.push_local 0

      g.push_literal "  first\n"
      g.string_dup

      g.push :self
      g.send :b, 0, true
      g.send :+, 1, false

      g.push_literal "  second\n"
      g.string_dup

      g.send :+, 1, false
      g.send :+, 1, false

      g.set_local 0
    end
  end

  relates <<-ruby do
<<-EOM
  blah
blah

  EOM
    ruby

    parse do
      [:str, "  blah\nblah\n\n"]
    end

    compile do |g|
      g.push_literal "  blah\nblah\n\n"
      g.string_dup
    end
  end

  relates <<-ruby do
<<'EOM'
  blah
blah
EOM
    ruby

    parse do
      [:str, "  blah\nblah\n"]
    end

    compile do |g|
      g.push_literal "  blah\nblah\n"
      g.string_dup
    end
  end

  relates "%(blah)" do
    parse do
      [:str, "blah"]
    end

    compile do |g|
      g.push_literal "blah"
      g.string_dup
    end
  end
end
