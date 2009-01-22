require File.dirname(__FILE__) + '/../spec_helper'

describe "A Valias node" do
  relates "alias $y $x" do
    parse do
      [:valias, :$y, :$x]
    end

    # valias
  end
end
