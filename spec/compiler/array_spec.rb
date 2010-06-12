require File.expand_path('../../spec_helper', __FILE__)

describe "An Array node" do
  relates '[1, :b, "c"]' do
    compile do |g|
      g.push 1
      g.push_literal :b
      g.push_literal "c"
      g.string_dup
      g.make_array 3
    end
  end

  relates "%w[a b c]" do
    compile do |g|
      g.push_literal "a"
      g.string_dup
      g.push_literal "b"
      g.string_dup
      g.push_literal "c"
      g.string_dup
      g.make_array 3
    end
  end

  relates '%w[a #{@b} c]' do
    compile do |g|
      g.push_literal "a"
      g.string_dup

      g.push_literal "\#{@b}"
      g.string_dup

      g.push_literal "c"
      g.string_dup
      g.make_array 3
    end
  end

  relates "%W[a b c]" do
    compile do |g|
      g.push_literal "a"
      g.string_dup
      g.push_literal "b"
      g.string_dup
      g.push_literal "c"
      g.string_dup
      g.make_array 3
    end
  end

  relates '%W[a #{@b} c]' do
    compile do |g|
      g.push_literal "a"
      g.string_dup

      g.push_ivar :@b
      g.send :to_s, 0, true
      g.string_build 1

      g.push_literal "c"
      g.string_dup
      g.make_array 3
    end
  end

  relates "[*[1]]" do
    compile do |g|
      g.push 1
      g.make_array 1
    end
  end

  relates "[*1]" do
    compile do |g|
      g.push 1
      g.cast_array
    end
  end

  relates "[[*1]]" do
    compile do |g|
      g.push 1
      g.cast_array
      g.make_array 1
    end
  end

  relates "[1, *2]" do
    compile do |g|
      g.push 1
      g.make_array 1

      g.push 2
      g.cast_array

      g.send :+, 1
    end
  end

  relates "[1, *c()]" do
    # TODO
  end

  relates <<-ruby do
      x = [2]
      [1, *x]
    ruby

    # TODO
  end
end
