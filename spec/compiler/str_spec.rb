require File.expand_path('../../spec_helper', __FILE__)

describe "A Str node" do
  relates '"x"' do
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

    compile(&str_concat)
  end

  relates '"before" " after"' do
    compile(&str_concat)
  end

  relates <<-ruby do
      "file = \#{__FILE__}\n"
    ruby

    compile do |g|
      g.push_literal "file = "

      g.push_scope
      g.send :active_path, 0
      g.send :to_s, 0, true

      g.push_literal "\n"

      g.string_build 3
    end
  end

  relates <<-ruby do
<<'EOM'.strip
  blah
blah
EOM
    ruby

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

    compile do |g|
      g.push_literal "  blah\nblah\n"
      g.string_dup
    end
  end

  relates "%(blah)" do
    compile do |g|
      g.push_literal "blah"
      g.string_dup
    end
  end
end
