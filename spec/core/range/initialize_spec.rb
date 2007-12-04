require File.dirname(__FILE__) + '/../../spec_helper'

describe "Range#initialize" do
  it "can't be called twice" do
    [0..1, 'a'..'b'].each do |range|
      lambda { range.send(:initialize, range.begin, range.end) }.should raise_error(NameError)
    end
  end
end
