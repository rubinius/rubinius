require File.expand_path('../../spec_helper', __FILE__)

describe "An Back_ref node" do
  relates "[$&, $`, $', $+]" do
    compile do |g|
      g.last_match 1, 0
      g.last_match 2, 0
      g.last_match 3, 0
      g.last_match 4, 0
      g.make_array 4
    end
  end
end
