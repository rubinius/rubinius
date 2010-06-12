require File.expand_path('../../spec_helper', __FILE__)

describe "A True node" do
  relates "true" do
    compile do |g|
      g.push :true
    end
  end
end
