require File.expand_path('../../spec_helper', __FILE__)

describe "A Hash node" do
  relates "{ 1 => 2, 3 => 4 }" do
    compile do |g|
      g.push_cpath_top
      g.find_const :Hash
      g.push 2
      g.send :new_from_literal, 1

      g.dup
      g.push 1
      g.push 2
      g.send :[]=, 2
      g.pop

      g.dup
      g.push 3
      g.push 4
      g.send :[]=, 2
      g.pop
    end
  end

  relates "{ 1 => (2 rescue 3) }" do
    compile do |g|
      g.push_cpath_top
      g.find_const :Hash
      g.push 1
      g.send :new_from_literal, 1

      g.dup
      g.push 1

      g.for_rescue do |rb|
        rb.body do
          g.push 2
        end

        rb.condition :StandardError do
          g.push 3
        end
      end

      g.send :[]=, 2
      g.pop
    end
  end

  relates "{ 1 => [*1] }" do
    compile do |g|
      g.push_cpath_top
      g.find_const :Hash
      g.push 1
      g.send :new_from_literal, 1

      g.dup
      g.push 1
      g.push 1
      g.cast_array
      g.send :[]=, 2
      g.pop
    end
  end

  relates <<-ruby do
      a = 1
      { :a => a }
    ruby

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop
      g.push_cpath_top
      g.find_const :Hash
      g.push 1
      g.send :new_from_literal, 1

      g.dup
      g.push_literal :a
      g.push_local 0
      g.send :[]=, 2
      g.pop
    end
  end
end
