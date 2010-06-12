require File.expand_path('../../spec_helper', __FILE__)

describe "A Return node" do
  relates "return" do
    compile do |g|
      g.push :nil
      g.ret
    end
  end

  relates "return 1" do
    compile do |g|
      g.push 1
      g.ret
    end
  end

  relates "return *1" do
    compile do |g|
      bottom = g.new_label

      g.push 1
      g.cast_array
      g.dup
      g.send :size, 0
      g.push 1
      g.send :>, 1
      g.git bottom

      g.push 0
      g.send :at, 1

      bottom.set!

      g.ret
    end
  end

  relates <<-ruby do
      x = 1, 2
      return *x
    ruby

    compile do |g|
      g.push 1
      g.push 2
      g.make_array 2
      g.set_local 0
      g.pop

      bottom = g.new_label

      g.push_local 0
      g.cast_array
      g.dup
      g.send :size, 0
      g.push 1
      g.send :>, 1
      g.git bottom

      g.push 0
      g.send :at, 1

      bottom.set!

      g.ret
    end
  end

  relates "return 1, 2, 3" do
    compile do |g|
      g.push 1
      g.push 2
      g.push 3
      g.make_array 3
      g.ret
    end
  end

  relates "return 1, 2, *c" do
    compile do |g|
      g.push 1
      g.push 2
      g.make_array 2

      g.push :self
      g.send :c, 0, true
      g.cast_array

      g.send :+, 1
      g.ret
    end
  end

  relates "return [*[1]]" do
    compile do |g|
      g.push 1
      g.make_array 1
      g.ret
    end
  end

  relates "return *[1]" do
    compile do |g|
      g.splatted_array
      g.ret
    end
  end
end
