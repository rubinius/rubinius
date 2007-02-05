require File.dirname(__FILE__) + '/../spec_helper'

context "Integer instance method" do
  specify "isspace should return true if self is ASCII whitespace" do
    example do
      @a = []
      "\tt\fi a\r\nt\v".each_byte { |b| @a << b.isspace }
      @a
    end.should == [true, false, true, false, true, false, true, true, false, true]
  end  
end
