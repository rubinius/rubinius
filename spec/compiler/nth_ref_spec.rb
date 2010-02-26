require File.dirname(__FILE__) + '/../spec_helper'

describe "An Nth_ref node" do
  relates "$1" do
    compile do |g|
      g.last_match 5, 0
    end
  end
end
