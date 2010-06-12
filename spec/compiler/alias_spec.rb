require File.expand_path('../../spec_helper', __FILE__)

describe "An Alias node" do
  alias_both = lambda do |g|
    in_class :X do |d|
      d.push_scope
      d.push_literal :y
      d.push_literal :x
      d.send :alias_method, 2, true
    end
  end

  relates <<-ruby do
      class X
        alias :y :x
      end
    ruby

    compile(&alias_both)
  end

  relates <<-ruby do
      class X
        alias y x
      end
    ruby

    compile(&alias_both)
  end

  relates <<-ruby do
      class X
        alias :"y\#{1}" :"x\#{2}"
      end
    ruby

    compile do |g|
      in_class :X do |d|
        d.push_scope

        d.push_literal "y"
        d.push 1
        d.send :to_s, 0, true
        d.string_build 2
        d.send :to_sym, 0, true

        d.push_literal "x"
        d.push 2
        d.send :to_s, 0, true
        d.string_build 2
        d.send :to_sym, 0, true

        d.send :alias_method, 2, true
      end
    end
  end
end
