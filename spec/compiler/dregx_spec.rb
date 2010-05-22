require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dregx node" do
  relates <<-ruby do
      /(\#{})/
    ruby

    compile do |g|
      g.push_cpath_top
      g.find_const :Regexp
      g.push_literal "("
      g.push_literal ")"
      g.string_build 2
      g.push 0
      g.send :new, 2
    end
  end

  relates "/x\#{(1 + 1)}y/" do
    compile do |g|
      g.push_cpath_top
      g.find_const :Regexp

      g.push_literal "x"    # 1

      g.push 1              # 2
      g.push 1
      g.send :+, 1, false
      g.send :to_s, 0, true

      g.push_literal "y"    # 3

      g.string_build 3

      g.push 0
      g.send :new, 2
    end
  end

  relates "/a\#{}b/" do
    compile do |g|
      g.push_cpath_top
      g.find_const :Regexp

      g.push_literal "a"
      g.push_literal "b"
      g.string_build 2

      g.push 0
      g.send :new, 2
    end
  end

  relates "/\#{@rakefile}/" do
    compile do |g|
      g.push_cpath_top
      g.find_const :Regexp

      g.push_ivar :@rakefile
      g.send :to_s, 0, true
      g.string_build 1

      g.push 0
      g.send :new, 2
    end
  end

  relates "/\#{1}/n" do
    compile do |g|
      g.push_cpath_top
      g.find_const :Regexp

      g.push 1
      g.send :to_s, 0, true
      g.string_build 1

      g.push 16
      g.send :new, 2
    end
  end

  relates "/\#{IAC}\#{SB}/no" do
    compile do |g|
      memoize do
        g.push_cpath_top
        g.find_const :Regexp

        g.push_const :IAC     # 1
        g.send :to_s, 0, true

        g.push_const :SB      # 2
        g.send :to_s, 0, true

        g.string_build 2

        g.push 16
        g.send :new, 2
      end
    end
  end

  relates "/x\#{(1 + 1)}y/o" do
    compile do |g|
      memoize do
        g.push_cpath_top
        g.find_const :Regexp

        g.push_literal "x"    # 1

        g.push 1              # 2
        g.push 1
        g.send :+, 1, false
        g.send :to_s, 0, true

        g.push_literal "y"    # 3

        g.string_build 3

        g.push 0
        g.send :new, 2
      end
    end
  end
end
