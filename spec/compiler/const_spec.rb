require File.dirname(__FILE__) + '/../spec_helper'

describe "A Const node" do
  relates "X" do
    compile do |g|
      g.push_const :X
    end
  end
end
