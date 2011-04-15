require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Numeric#modulo" do
  ruby_version_is "".."1.9" do
    it "returns the result of calling self#% with other" do
      obj = NumericSpecs::Subclass.new
      obj.should_receive(:%).with(20).and_return(:result)

      obj.modulo(20).should == :result
    end
  end

  ruby_version_is "1.9" do
    it "returns the result of self#- (self#/ other) * other" do
      obj = NumericSpecs::Subclass.new
      obj.should_receive(:/).with(15).and_return(13)
      obj.should_receive(:-).with(195).and_return(5)
      # 200 % 15 == 5
      obj.modulo(15).should == 5
    end
  end
end

ruby_version_is "1.9" do
  describe "Numeric#%" do
    it "needs to be reviewed for spec completeness"
  end
end

ruby_version_is "1.9" do
  describe "Numeric#modulo" do
    it "needs to be reviewed for spec completeness"
  end
end
