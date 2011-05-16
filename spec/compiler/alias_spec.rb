require File.expand_path('../../spec_helper', __FILE__)

describe "An Alias node" do
  alias_both = lambda do |g|
    g.push_scope
    g.push_literal :y
    g.push_literal :x
    g.send :alias_method, 2, true
  end

  relates "alias :y :x" do
    compile(&alias_both)
  end

  relates "alias y x" do
    compile(&alias_both)
  end

  relates <<-ruby do
      alias :"y\#{1}" :"x\#{2}"
    ruby

    compile do |g|
      g.push_scope

      g.push_literal "y"
      g.push 1
      g.meta_to_s
      g.string_build 2
      g.send :to_sym, 0, true

      g.push_literal "x"
      g.push 2
      g.meta_to_s
      g.string_build 2
      g.send :to_sym, 0, true

      g.send :alias_method, 2, true
    end
  end
end
