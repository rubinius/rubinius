require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dregx node" do
  relates <<-ruby do
      /(\#{})/
    ruby

    parse do
      [:dregx, "(", [:str, ""], [:str, ")"]]
    end

    compile do |g|
      g.push_const :Regexp
      g.push_literal ")"
      g.string_dup
      g.push_literal ""
      g.string_dup
      g.push_literal "("
      g.string_dup
      g.string_append
      g.string_append
      g.push 0
      g.send :new, 2
    end
  end

  relates "/x\#{(1 + 1)}y/" do
    parse do
      [:dregx,
       "x",
       [:evstr, [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]],
       [:str, "y"]]
    end

    compile do |g|
      g.push_const :Regexp

      g.push_literal "y"    # 1
      g.string_dup

      g.push 1              # 2
      g.push 1
      g.send :+, 1, false
      g.send :to_s, 0, true

      g.push_literal "x"    # 3
      g.string_dup

      2.times do
        g.string_append
      end

      g.push 0
      g.send :new, 2
    end
  end

  relates "/a\#{}b/" do
    parse do
      [:dregx, "a", [:str, ""], [:str, "b"]]
    end

    compile do |g|
      g.push_const :Regexp

      g.push_literal "b"
      g.string_dup

      g.push_literal ""
      g.string_dup

      g.push_literal "a"
      g.string_dup

      g.string_append
      g.string_append

      g.push 0
      g.send :new, 2
    end
  end

  relates "/\#{@rakefile}/" do
    parse do
      [:dregx, "", [:evstr, [:ivar, :@rakefile]]]
    end

    compile do |g|
      g.push_const :Regexp

      g.push_ivar :@rakefile
      g.send :to_s, 0, true

      g.push_literal ""
      g.string_dup

      g.string_append

      g.push 0
      g.send :new, 2
    end
  end

  relates "/\#{1}/n" do
    parse do
      [:dregx, "", [:evstr, [:lit, 1]], 16]
    end

    compile do |g|
      g.push_const :Regexp

      g.push 1
      g.send :to_s, 0, true

      g.push_literal ""
      g.string_dup

      g.string_append

      g.push 16
      g.send :new, 2
    end
  end

  relates "/\#{IAC}\#{SB}/no" do
    parse do
      [:dregx_once, "", [:evstr, [:const, :IAC]], [:evstr, [:const, :SB]], 16]
    end

    compile do |g|
      memoize do
        g.push_const :Regexp

        g.push_const :SB      # 1
        g.send :to_s, 0, true

        g.push_const :IAC     # 2
        g.send :to_s, 0, true

        g.push_literal ""     # 3
        g.string_dup

        2.times do
          g.string_append
        end

        g.push 16
        g.send :new, 2
      end
    end
  end

  relates "/x\#{(1 + 1)}y/o" do
    parse do
      [:dregx_once,
       "x",
       [:evstr, [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]],
       [:str, "y"]]
    end

    compile do |g|
      memoize do
        g.push_const :Regexp

        g.push_literal "y"    # 1
        g.string_dup

        g.push 1              # 2
        g.push 1
        g.send :+, 1, false
        g.send :to_s, 0, true

        g.push_literal "x"    # 3
        g.string_dup

        2.times do
          g.string_append
        end

        g.push 0
        g.send :new, 2
      end
    end
  end
end
