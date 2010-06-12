require File.expand_path('../../spec_helper', __FILE__)

describe "A False node" do
  relates "false" do
    compile do |g|
      g.push :false
    end
  end
end
