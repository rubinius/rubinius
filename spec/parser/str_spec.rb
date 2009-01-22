require File.dirname(__FILE__) + '/../spec_helper'

describe "A Str node" do
  relates '"x"' do

    parse do
      [:str, "x"]
    end

    # str
  end

  relates <<-ruby do
      "before" \
      " after"
    ruby

    parse do
      [:str, "before after"]
    end

    # str concat newline
  end

  relates '"before" " after"' do
    parse do
      [:str, "before after"]
    end

    # str concat space
  end

  relates <<-ruby do
      "file = \#{__FILE__}\n"
    ruby

    parse do
      [:str, "file = (eval)\n"]
    end

    # str interp file
  end

  relates <<-ruby do
<<'EOM'.strip
  blah
blah
EOM
    ruby

    parse do
      [:call, [:str, "  blah\nblah\n"], :strip, [:arglist]]
    end

    # str heredoc call
  end

  relates <<-ruby do
a += <<-H1 + b + <<-H2
  first
H1
  second
H2
    ruby

    parse do
      [:lasgn,
       :a,
       [:call,
        [:lvar, :a],
        :+,
        [:arglist,
         [:call,
          [:call, [:str, "  first\n"], :+, [:arglist, [:call, nil, :b, [:arglist]]]],
          :+,
          [:arglist, [:str, "  second\n"]]]]]]
    end

    # str heredoc double
  end

  relates <<-ruby do
      <<-EOM
        blah
      blah

        EOM
    ruby

    parse do
      [:str, "        blah\n      blah\n\n"]
    end

    # str heredoc indent
  end

  relates <<-ruby do
<<'EOM'
  blah
blah
EOM
    ruby

    parse do
      [:str, "  blah\nblah\n"]
    end

    # str heredoc
  end
end
