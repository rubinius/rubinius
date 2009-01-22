require File.dirname(__FILE__) + '/../spec_helper'

describe "A Defined node" do
  relates "defined? $x" do
    parse do
      [:defined, [:gvar, :$x]]
    end
  end
end
