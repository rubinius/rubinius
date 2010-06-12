require File.expand_path('../../spec_helper', __FILE__)

describe "A Const node" do
  relates "X" do
    compile do |g|
      g.push_const :X
    end
  end
end
