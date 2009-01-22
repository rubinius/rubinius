require File.dirname(__FILE__) + '/../spec_helper'

describe "A Xstr node" do
  relates "`touch 5`" do
    parse do
      [:xstr, "touch 5"]
    end

    # xstr
  end
end
