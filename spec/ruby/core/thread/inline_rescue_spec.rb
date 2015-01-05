require File.expand_path('../../../shared/kernel/raise', __FILE__)

describe "inline rescue" do
  it "properly executes compound assignment operators" do
    val = 'hello'
    val ||= 'goodbye' rescue nil
    val.should == 'hello'
  end
end
