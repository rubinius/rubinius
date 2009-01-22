require File.dirname(__FILE__) + '/../spec_helper'

describe "A Regex node" do
  relates "str.split(//i)" do
    parse do
      [:call,
        [:call, nil, :str, [:arglist]],
        :split,
        [:arglist, [:regex, "", 1]]]
    end

    # lit regexp i wwtt
  end

  relates "/x/n" do
    parse do
      [:regex, "x", 16]
    end

    # lit regexp n
  end

  relates "/x/o" do
    parse do
      [:regex, "x", 0]
    end

    # lit regexp once
  end

  relates "/x/" do
    parse do
      [:regex, "x", 0]
    end

    # lit regexp
  end

end
