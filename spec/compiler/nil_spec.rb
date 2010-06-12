require File.expand_path('../../spec_helper', __FILE__)

describe "A Nil node" do
  relates "nil" do
    compile do |g|
      g.push :nil
    end
  end
end
