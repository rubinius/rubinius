require File.dirname(__FILE__) + '/../../spec_helper'

describe "File#initialize" do
  it "needs to be reviewed for spec completeness"
end

ruby_version_is '1.9' do
  describe "File#initialize" do
    it 'accepts encoding options in mode parameter' do
      io = File.new(__FILE__, 'r:UTF-8:iso-8859-1')
      io.external_encoding.to_s.should == 'UTF-8'
      io.internal_encoding.to_s.should == 'ISO-8859-1'
    end
    
    it 'accepts encoding options as a hash parameter' do
      io = File.new(__FILE__, 'r', :encoding => 'UTF-8:iso-8859-1')
      io.external_encoding.to_s.should == 'UTF-8'
      io.internal_encoding.to_s.should == 'ISO-8859-1'
    end
  end
end
