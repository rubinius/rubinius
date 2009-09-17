require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dstr node" do
  relates <<-ruby do
      "hello \#{}"
    ruby

    parse do
      [:dstr, "hello ", [:str, ""]]
    end

    compile do |g|
      g.push_literal ""
      g.string_dup
      g.push_literal "hello "
      g.string_dup
      g.string_append
    end
  end

  relates <<-ruby do
      argl = 1
      "x\#{argl}y"
    ruby

    parse do
      [:block,
       [:lasgn, :argl, [:lit, 1]],
       [:dstr, "x", [:evstr, [:lvar, :argl]], [:str, "y"]]]
    end

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop

      g.push_literal "y"    # 1
      g.string_dup

      g.push_local 0        # 2
      g.send :to_s, 0, true

      g.push_literal "x"    # 3
      g.string_dup

      2.times do
        g.string_append
      end
    end
  end

  relates <<-ruby do
      argl = 1
      "x\#{("%.2f" % 3.14159)}y"
    ruby

    parse do
      [:block,
       [:lasgn, :argl, [:lit, 1]],
       [:dstr,
        "x",
        [:evstr, [:call, [:str, "%.2f"], :%, [:arglist, [:lit, 3.14159]]]],
        [:str, "y"]]]
    end

    compile do |g|
      g.push 1
      g.set_local 0
      g.pop

      g.push_literal "y"    # 1
      g.string_dup

      g.push_literal "%.2f" # 2
      g.string_dup
      g.push_literal 3.14159
      g.send :%, 1, false
      g.send :to_s, 0, true

      g.push_literal "x"    # 3
      g.string_dup

      2.times do
        g.string_append
      end
    end
  end

  relates <<-ruby do
      max = 2
      argl = 1
      "x\#{("%.\#{max}f" % 3.14159)}y"
    ruby

    parse do
      [:block,
       [:lasgn, :max, [:lit, 2]],
       [:lasgn, :argl, [:lit, 1]],
       [:dstr,
        "x",
        [:evstr,
         [:call,
          [:dstr, "%.", [:evstr, [:lvar, :max]], [:str, "f"]],
          :%,
          [:arglist, [:lit, 3.14159]]]],
        [:str, "y"]]]
    end

    compile do |g|
      g.push 2
      g.set_local 0
      g.pop
      g.push 1
      g.set_local 1
      g.pop

      g.push_literal "y"  # - # 1
      g.string_dup

      g.push_literal "f"  # 1
      g.string_dup

      g.push_local 0      # 2
      g.send :to_s, 0, true

      g.push_literal "%." # 3
      g.string_dup

      2.times do
        g.string_append
      end

      g.push_literal 3.14159      # - # 2
      g.send :%, 1, false
      g.send :to_s, 0, true

      g.push_literal "x"  # - # 3
      g.string_dup

      2.times do
        g.string_append
      end
    end
  end

  relates '"#{22}aa" "cd#{44}" "55" "#{66}"' do
    parse do
      [:dstr,
       "",
       [:evstr, [:lit, 22]],
       [:str, "aa"],
       [:str, "cd"],
       [:evstr, [:lit, 44]],
       [:str, "55"],
       [:evstr, [:lit, 66]]]
    end

    compile do |g|
      g.push 66             # 1
      g.send :to_s, 0, true

      g.push_literal "55"   # 2
      g.string_dup

      g.push 44             # 3
      g.send :to_s, 0, true

      g.push_literal "cd"   # 4
      g.string_dup

      g.push_literal "aa"   # 5
      g.string_dup

      g.push 22             # 6
      g.send :to_s, 0, true

      g.push_literal ""     # 7
      g.string_dup

      6.times do
        g.string_append
      end
    end
  end

  relates '"a #$global b #@ivar c #@@cvar d"' do
    parse do
      [:dstr,
       "a ",
       [:evstr, [:gvar, :$global]],
       [:str, " b "],
       [:evstr, [:ivar, :@ivar]],
       [:str, " c "],
       [:evstr, [:cvar, :@@cvar]],
       [:str, " d"]]
    end

    compile do |g|
      g.push_literal " d"           # 1
      g.string_dup

      g.push_scope                  # 2
      g.push_literal :@@cvar
      g.send :class_variable_get, 1
      g.send :to_s, 0, true

      g.push_literal " c "          # 3
      g.string_dup

      g.push_ivar :@ivar            # 4
      g.send :to_s, 0, true

      g.push_literal " b "          # 5
      g.string_dup

      g.push_const :Rubinius        # 6
      g.find_const :Globals
      g.push_literal :$global
      g.send :[], 1
      g.send :to_s, 0, true

      g.push_literal "a "           # 7
      g.string_dup

      6.times do
        g.string_append
      end
    end
  end

  relates <<-ruby do
<<EOM
  blah
\#{1 + 1}blah
EOM
    ruby

    parse do
      [:dstr,
       "  blah\n",
       [:evstr, [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]],
       [:str, "blah\n"]]
    end

    compile do |g|
      g.push_literal "blah\n"   # 1
      g.string_dup

      g.push 1                  # 2
      g.push 1
      g.send :+, 1, false
      g.send :to_s, 0, true

      g.push_literal "  blah\n" # 3
      g.string_dup

      2.times do
        g.string_append
      end
    end
  end

  relates <<-ruby do
<<-EOF
def test_\#{action}_valid_feed
EOF
    ruby

    parse do
      [:dstr,
       "def test_",
       [:evstr, [:call, nil, :action, [:arglist]]],
       [:str, "_valid_feed\n"]]
    end

    compile do |g|
      g.push_literal "_valid_feed\n" # 1
      g.string_dup

      g.push :self                   # 2
      g.send :action, 0, true
      g.send :to_s, 0, true

      g.push_literal "def test_"     # 3
      g.string_dup

      2.times do
        g.string_append
      end
    end
  end

  relates <<-ruby do
<<-EOF
s1 '\#{RUBY_PLATFORM}' s2
\#{__FILE__}
EOF
    ruby

    parse do
      [:dstr,
       "s1 '",
       [:evstr, [:const, :RUBY_PLATFORM]],
       [:str, "' s2\n"],
       [:evstr, [:file]],
       [:str, "\n"]]
    end

    compile do |g|
      g.push_literal "\n"         # 1
      g.string_dup

      g.push_scope                # 2
      g.send :active_path, 0
      g.send :to_s, 0, true

      g.push_literal "' s2\n"     # 3
      g.string_dup

      g.push_const :RUBY_PLATFORM # 4
      g.send :to_s, 0, true

      g.push_literal "s1 '"       # 5
      g.string_dup

      4.times do
        g.string_append
      end
    end
  end

  relates "%Q[before [\#{nest}] after]" do
    parse do
      [:dstr,
       "before [",
       [:evstr, [:call, nil, :nest, [:arglist]]],
       [:str, "] after"]]
    end

    compile do |g|
      g.push_literal "] after"  # 1
      g.string_dup

      g.push :self              # 2
      g.send :nest, 0, true
      g.send :to_s, 0, true

      g.push_literal "before [" # 3
      g.string_dup

      2.times do
        g.string_append
      end
    end
  end

  relates '"#{"blah"}#{__FILE__}:#{__LINE__}: warning: #{$!.message} (#{$!.class})"' do
    parse do
      [:dstr,
       "blah",
       [:evstr, [:file]],
       [:str, ":"],
       [:evstr, [:lit, 1]],
       [:str, ": warning: "],
       [:evstr, [:call, [:gvar, :$!], :message, [:arglist]]],
       [:str, " ("],
       [:evstr, [:call, [:gvar, :$!], :class, [:arglist]]],
       [:str, ")"]]
    end

    compile do |g|
      g.push_literal ")"           # 1
      g.string_dup

      g.push_exception             # 2
      g.send :class, 0, false
      g.send :to_s, 0, true

      g.push_literal " ("          # 3
      g.string_dup

      g.push_exception             # 4
      g.send :message, 0, false
      g.send :to_s, 0, true

      g.push_literal ": warning: " # 5
      g.string_dup

      g.push 1                     # 6
      g.send :to_s, 0, true

      g.push_literal ":"           # 7
      g.string_dup
      g.push_scope                 # 8
      g.send :active_path, 0
      g.send :to_s, 0, true
      g.push_literal "blah"
      g.string_dup

      8.times do
        g.string_append
      end
    end
  end

  relates '"before #{from} middle #{to} (#{__FILE__}:#{__LINE__})"' do
    parse do
      [:dstr,
       "before ",
       [:evstr, [:call, nil, :from, [:arglist]]],
       [:str, " middle "],
       [:evstr, [:call, nil, :to, [:arglist]]],
       [:str, " ("],
       [:evstr, [:file]],
       [:str, ":"],
       [:evstr, [:lit, 1]],
       [:str, ")"]]
    end

    compile do |g|
      g.push_literal ")"        # 1
      g.string_dup

      g.push 1                  # 2
      g.send :to_s, 0, true

      g.push_literal ":"        # 3
      g.string_dup

      g.push_scope              # 4
      g.send :active_path, 0
      g.send :to_s, 0, true

      g.push_literal " ("       # 5
      g.string_dup

      g.push :self              # 6
      g.send :to, 0, true
      g.send :to_s, 0, true

      g.push_literal " middle " # 7
      g.string_dup

      g.push :self              # 8
      g.send :from, 0, true
      g.send :to_s, 0, true

      g.push_literal "before "  # 9
      g.string_dup

      8.times do
        g.string_append
      end
    end
  end
end
