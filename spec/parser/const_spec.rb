require File.dirname(__FILE__) + '/../spec_helper'

describe "A Const node" do
  relates "X" do
    parse do
      [:const, :X]
    end

    # const
  end
end
