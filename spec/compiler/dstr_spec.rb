require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dstr node" do
  relates <<-ruby do
      "\#{}"
    ruby

    compile do |g|
      g.push_literal ""
      g.string_dup
    end
  end

  relates <<-ruby do
      "\#{}\#{}"
    ruby

    compile do |g|
      g.push_literal ""
      g.string_dup
    end
  end

  relates <<-ruby do
      "\#{}hello\#{}"
    ruby

    compile do |g|
      g.push_literal "hello"
      g.string_build 1
    end
  end

  relates <<-ruby do
      "hello \#{}"
    ruby

    compile do |g|
      g.push_literal "hello "
      g.string_dup
    end
  end

  relates <<-ruby do
      "\#{} hello"
    ruby

    compile do |g|
      g.push_literal " hello"
      g.string_build 1
    end
  end

  relates <<-ruby do
      "\#{a}"
    ruby

    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.send :to_s, 0, true
      g.string_build 1
    end
  end

  relates <<-ruby do
      "hello \#{a}"
    ruby

    compile do |g|
      g.push_literal "hello "
      g.push :self
      g.send :a, 0, true
      g.send :to_s, 0, true
      g.string_build 2
    end
  end

  relates <<-ruby do
      "\#{a} hello"
    ruby

    compile do |g|
      g.push :self
      g.send :a, 0, true
      g.send :to_s, 0, true
      g.push_literal " hello"
      g.string_build 2
    end
  end

  relates <<-ruby do
      argl = 1
      "x\#{argl}y"
    ruby

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop

      g.push_literal "x"    # 1

      g.push_local 0        # 2
      g.send :to_s, 0, true

      g.push_literal "y"    # 3

      g.string_build 3
    end
  end

  relates <<-ruby do
      argl = 1
      "x\#{("%.2f" % 3.14159)}y"
    ruby

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop

      g.push_literal "x"    # 1

      g.push_literal "%.2f" # 2
      g.string_dup
      g.push_literal 3.14159
      g.send :%, 1, false
      g.send :to_s, 0, true

      g.push_literal "y"    # 3

      g.string_build 3
    end
  end

  relates <<-ruby do
      max = 2
      argl = 1
      "x\#{("%.\#{max}f" % 3.14159)}y" # "
    ruby

    compile do |g|
      g.push 2
      g.set_local 0
      g.pop
      g.push 1
      g.set_local 1
      g.pop

      g.push_literal "x"  # - # 1

      g.push_literal "%." # 1

      g.push_local 0      # 2
      g.send :to_s, 0, true

      g.push_literal "f"  # 3

      g.string_build 3

      g.push_literal 3.14159      # - # 2
      g.send :%, 1, false
      g.send :to_s, 0, true

      g.push_literal "y"  # - # 3

      g.string_build 3
    end
  end

  relates '"#{22}aa" "cd#{44}" "55" "#{66}"' do
    compile do |g|
      g.push 22             # 1
      g.send :to_s, 0, true

      g.push_literal "aa"   # 2
      g.push_literal "cd"   # 3

      g.push 44             # 4
      g.send :to_s, 0, true

      g.push_literal "55"   # 5

      g.push 66             # 6
      g.send :to_s, 0, true

      g.string_build 6
    end
  end

  relates '"a #$global b #@ivar c #@@cvar d"' do
    compile do |g|
      g.push_literal "a "           # 1

      g.push_const :Rubinius        # 2
      g.find_const :Globals
      g.push_literal :$global
      g.send :[], 1
      g.send :to_s, 0, true

      g.push_literal " b "          # 3

      g.push_ivar :@ivar            # 4
      g.send :to_s, 0, true

      g.push_literal " c "          # 5

      g.push_scope                  # 6
      g.push_literal :@@cvar
      g.send :class_variable_get, 1
      g.send :to_s, 0, true

      g.push_literal " d"           # 7

      g.string_build 7
    end
  end

  relates <<-ruby do
<<EOM
  foo
\#{1 + 1}blah
EOM
    ruby

    compile do |g|
      g.push_literal "  foo\n" # 1

      g.push 1                # 2
      g.push 1
      g.send :+, 1, false
      g.send :to_s, 0, true

      g.push_literal "blah\n" # 3

      g.string_build 3
    end
  end

  relates <<-ruby do
<<-EOF
def test_\#{action}_valid_feed
EOF
    ruby

    compile do |g|
      g.push_literal "def test_"     # 1

      g.push :self                   # 2
      g.send :action, 0, true
      g.send :to_s, 0, true

      g.push_literal "_valid_feed\n" # 1

      g.string_build 3
    end
  end

  relates <<-ruby do
<<-EOF
s1 '\#{RUBY_PLATFORM}' s2
\#{__FILE__}
EOF
    ruby

    compile do |g|
      g.push_literal "s1 '"       # 1

      g.push_const :RUBY_PLATFORM # 2
      g.send :to_s, 0, true

      g.push_literal "' s2\n"     # 3

      g.push_scope                # 4
      g.send :active_path, 0
      g.send :to_s, 0, true

      g.push_literal "\n"         # 5

      g.string_build 5
    end
  end

  relates "%Q[before [\#{nest}] after]" do
    compile do |g|
      g.push_literal "before [" # 1

      g.push :self              # 2
      g.send :nest, 0, true
      g.send :to_s, 0, true

      g.push_literal "] after"  # 3

      g.string_build 3
    end
  end

  relates '"#{"blah"}#{__FILE__}:#{__LINE__}: warning: #{$!.message} (#{$!.class})"' do
    compile do |g|
      g.push_literal "blah"        # 1

      g.push_scope                 # 2
      g.send :active_path, 0
      g.send :to_s, 0, true

      g.push_literal ":"           # 3

      g.push 1                     # 4
      g.send :to_s, 0, true

      g.push_literal ": warning: " # 5

      g.push_current_exception             # 6
      g.send :message, 0, false
      g.send :to_s, 0, true

      g.push_literal " ("          # 7

      g.push_current_exception             # 8
      g.send :class, 0, false
      g.send :to_s, 0, true

      g.push_literal ")"           # 9

      g.string_build 9
    end
  end

  relates '"before #{from} middle #{to} (#{__FILE__}:#{__LINE__})"' do
    compile do |g|
      g.push_literal "before "  # 1

      g.push :self              # 2
      g.send :from, 0, true
      g.send :to_s, 0, true

      g.push_literal " middle " # 3

      g.push :self              # 4
      g.send :to, 0, true
      g.send :to_s, 0, true

      g.push_literal " ("       # 5

      g.push_scope              # 6
      g.send :active_path, 0
      g.send :to_s, 0, true

      g.push_literal ":"        # 7

      g.push 1                  # 8
      g.send :to_s, 0, true

      g.push_literal ")"        # 9

      g.string_build 9
    end
  end
end
