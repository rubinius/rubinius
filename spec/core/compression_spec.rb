require File.dirname(__FILE__) + '/../spec_helper'

context "Compression::ZLib" do
  specify "inflate should uncompress data" do
    example do
      @data = Compression::ZLib.deflate("blah")
      puts Compression::ZLib.inflate(@data)
    end.should == 'blah'
  end
  
  specify "deflate should convert other to compressed data" do
    example do
      puts Compression::ZLib.deflate("blah")
    end.should == "x\234K\312I\314\000\000\003\372\001\230"
  end
end