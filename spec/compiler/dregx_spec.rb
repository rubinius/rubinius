require File.expand_path('../../spec_helper', __FILE__)

describe "A Dregx node" do
  ruby_version_is ""..."1.9" do
    # TODO: Fix 1.8 parser
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
  end

  ruby_version_is "1.9" do
    relates <<-ruby do
        /(\#{})/
      ruby

      compile do |g|
        g.push_cpath_top
        g.find_const :Regexp
        g.push_literal "("
        g.push :nil
        g.meta_to_s
        g.push_literal ")"
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
        g.push :nil
        g.meta_to_s
        g.push_literal "b"
        g.string_build 3

        g.push 0
        g.send :new, 2
      end
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
      g.meta_to_s

      g.push_literal "y"    # 3

      g.string_build 3

      g.push 0
      g.send :new, 2
    end
  end

  relates "/\#{@rakefile}/" do
    compile do |g|
      g.push_cpath_top
      g.find_const :Regexp

      g.push_ivar :@rakefile
      g.meta_to_s
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
      g.meta_to_s
      g.string_build 1

      g.push 512
      g.send :new, 2
    end
  end

  relates "/\#{IAC}\#{SB}/no" do
    compile do |g|
      memoize do
        g.push_cpath_top
        g.find_const :Regexp

        g.push_const :IAC     # 1
        g.meta_to_s

        g.push_const :SB      # 2
        g.meta_to_s

        g.string_build 2

        g.push 512
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
        g.meta_to_s

        g.push_literal "y"    # 3

        g.string_build 3

        g.push 0
        g.send :new, 2
      end
    end
  end
end
