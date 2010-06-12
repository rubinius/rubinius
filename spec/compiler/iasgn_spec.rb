require File.expand_path('../../spec_helper', __FILE__)

describe "A Iasgn node" do
  relates "@a = 4" do
    compile do |g|
      g.push 4
      g.set_ivar :@a
    end
  end

  relates "@a = *[1]" do
    # iasgn splat
  end

  relates <<-ruby do
      a = 1
      @a = a
    ruby

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop
      g.push_local 0
      g.set_ivar :@a
    end
  end
end
