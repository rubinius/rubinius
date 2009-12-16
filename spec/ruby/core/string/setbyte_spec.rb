require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.8.8" do
  describe "String#setbyte" do
    it "returns an Integer" do
      "a".setbyte(0,1).should be_kind_of(Integer)
    end
    
    it "modifies the receiver" do
      str = "glark"
      old_id = str.object_id
      str.setbyte(0,88)
      str.object_id.should == old_id
    end

    it "changes the byte at the given index to the new byte" do
      str = "a"
      str.setbyte(0,98)
      str.should == 'b'
    end

    it "allows changing bytes in multi-byte characters" do
      str = "\u{915}"
      str.setbyte(1,254)
      str.getbyte(1).should == 254
    end

    it "can invalidate a String's encoding" do
      str = "glark"
      str.valid_encoding?.should be_true
      str.setbyte(2,253)
      str.valid_encoding?.should be_false
    end

    it "regards a negative index as counting from the end of the String" do
      str = "hedgehog"
      str.setbyte(-3, 108)
      str.should == "hedgelog"
    end

    it "raises an IndexError unless the index is inside the String" do
      lambda { "?".setbyte(1,97) }.should raise_error(IndexError)
    end

    it "raises a RuntimeError if self is frozen" do
      str = "cold".freeze
      str.frozen?.should be_true
      lambda { str.setbyte(3,96) }.should raise_error(RuntimeError)
    end
    
    it "raises a TypeError unless the second argument is an Integer" do
      lambda { "a".setbyte(0,'a') }.should raise_error(TypeError)
    end  
  end
end
