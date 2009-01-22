require File.dirname(__FILE__) + '/../spec_helper'

describe "A Gvar node" do
  relates "$x" do
    parse do
      [:gvar, :$x]
    end

    # gvar
  end

  relates "$stderr" do
    parse do
      [:gvar, :$stderr]
    end

    # global
  end

  relates "$__blah" do
    parse do
      [:gvar, :$__blah]
    end

    # gvar underscore blah
  end

  relates "$_" do
    parse do
      [:gvar, :$_]
    end

    # gvar underscore
  end
end
