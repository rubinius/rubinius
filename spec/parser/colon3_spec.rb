require File.dirname(__FILE__) + '/../spec_helper'

describe "A Colon3 node" do
  relates "::X" do
    parse do
      [:colon3, :X]
    end

  end
end
