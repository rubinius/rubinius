require File.dirname(__FILE__) + '/../spec_helper'

describe "A Dregx node" do
  relates "/x\#{(1 + 1)}y/" do
    parse do
      [:dregx,
       "x",
       [:evstr, [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]],
       [:str, "y"]]
    end

    # dregx
  end

  relates "/a\#{}b/" do
    parse do
      [:dregx, "a", [:evstr], [:str, "b"]]
    end

    # dregx interp empty
  end

  relates "/\#{@rakefile}/" do
    parse do
      [:dregx, "", [:evstr, [:ivar, :@rakefile]]]
    end

    # dregx interp
  end

  relates "/\#{1}/n" do
    parse do
      [:dregx, "", [:evstr, [:lit, 1]], 16]
    end

    # dregx n
  end

  relates "/\#{IAC}\#{SB}/no" do
    parse do
      [:dregx_once, "", [:evstr, [:const, :IAC]], [:evstr, [:const, :SB]], 16]
    end

    # dregx once n interp
  end

  relates "/x\#{(1 + 1)}y/o" do
    parse do
      [:dregx_once,
       "x",
       [:evstr, [:call, [:lit, 1], :+, [:arglist, [:lit, 1]]]],
       [:str, "y"]]
    end

    # dregx once
  end
end
