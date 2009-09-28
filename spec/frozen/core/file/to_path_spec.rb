require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "File#to_path" do
    
    it "returns a String" do
      File.new(__FILE__).to_path.should be_an_instance_of(String)
    end

    it "doesn't normalise the path it returns" do
      Dir.chdir(File.dirname(__FILE__)) do
        unorm ='./' + File.basename(__FILE__)
        File.new(unorm).to_path.should == unorm
      end
    end

    it "doesn't expand the path it returns" do
      File.new('../').to_path.should == '../'
    end

    it "doesn't absolute-ise the path it returns" do
      Dir.chdir(File.dirname(__FILE__)) do
        rel_path = File.basename(__FILE__)
        File.new(rel_path).to_path.should == rel_path
      end
    end

    it "preserves the encoding of the path" do
      path = File.new(__FILE__.encode('euc-jp')).to_path
      path.encoding.should == Encoding::EUC_JP
    end
  end
end
