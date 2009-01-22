require File.dirname(__FILE__) + '/../spec_helper'

describe "A Lit node" do
  relates "0b1111" do
    parse do
      [:lit, 15]
    end

    # lit binary
  end

  relates "1.1" do
    parse do
      [:lit, 1.1]
    end

    # lit float
  end

  relates "0xffffffff" do
    parse do
      [:lit, 4294967295]
    end

    # lit hex
  end

  relates "0xffff_ffff" do
    parse do
      [:lit, 4294967295]
    end

    # lit hex underscore
  end

  relates "07654" do
    parse do
      [:lit, 4012]
    end

    # lit octal
  end

  relates "-1" do
    parse do
      [:lit, -1]
    end

    # lit long negative
  end

  relates "1" do
    parse do
      [:lit, 1]
    end

    # lit long
  end

  relates "8000_0000" do
    parse do
      [:lit, 80000000]
    end

    # lit long underscore
  end

  relates ":x" do
    parse do
      [:lit, :x]
    end

    # lit sym
  end

  relates ":\"*args\"" do
    parse do
      [:lit, :"*args"]
    end

    # lit sym splat
  end
end
