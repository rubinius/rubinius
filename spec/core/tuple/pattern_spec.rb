require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple.pattern" do
  it "returns a Tuple of specified length initialized to object" do
    t = Rubinius::Tuple.pattern(5, -1)
    t.size.should == 5
    t.each { |e| e.should == -1 }
  end
end
