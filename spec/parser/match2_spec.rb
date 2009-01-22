require File.dirname(__FILE__) + '/../spec_helper'

describe "A Match2 node" do
  relates '/x/ =~ "blah"' do
    parse do
      [:match2, [:regex, "x", 0], [:str, "blah"]]
    end

    # match2
  end
end
