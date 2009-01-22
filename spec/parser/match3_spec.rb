require File.dirname(__FILE__) + '/../spec_helper'

describe "A Match3 node" do
  relates '"blah" =~ /x/' do
    parse do
      [:match3, [:regex, "x", 0], [:str, "blah"]]
    end

    # match3
  end
end
