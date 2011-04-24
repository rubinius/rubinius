require File.expand_path('../../fixtures/common', __FILE__)

describe :to_s, :shared => true do

  it "returns the self's name if no message is set" do
    Exception.new.send(@method).should == 'Exception'
    ExceptionSpecs::Exceptional.new.send(@method).should == 'ExceptionSpecs::Exceptional'
  end

  it "returns self's message if set" do
    ExceptionSpecs::Exceptional.new('!!').send(@method).should == '!!'
  end

end
