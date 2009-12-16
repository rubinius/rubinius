require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "File#to_path" do

    it "returns a String" do
      File.new(__FILE__).to_path.should be_an_instance_of(String)
    end

    it "does not normalise the path it returns" do
      Dir.chdir(File.dirname(__FILE__)) do
        unorm ='./' + File.basename(__FILE__)
        File.new(unorm).to_path.should == unorm
      end
    end

    it "does not expand the path it returns" do
      File.new('../').to_path.should == '../'
    end

    it "does not absolute-ise the path it returns" do
      Dir.chdir(File.dirname(__FILE__)) do
        rel_path = File.basename(__FILE__)
        File.new(rel_path).to_path.should == rel_path
      end
    end

    it "does not preserve the encoding of the path" do
      path = File.new(__FILE__.encode('euc-jp')).to_path
      path.encoding.should == Encoding.find(:filesystem)
    end
  end
end
