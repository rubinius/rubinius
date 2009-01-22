require File.dirname(__FILE__) + '/../spec_helper'

describe "A Not node" do
  relates "(not true)" do
    parse do
      [:not, [:true]]
    end

    # not
  end
end
