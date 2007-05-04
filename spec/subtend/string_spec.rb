require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/string/subtend_string'

context "SubtendString" do
  setup do
    @s = SubtendString.new
  end
    
  specify "rb_str_new should return a new string object" do
    # Hardcoded to pass const char * = "hello"
    @s.say_hello.should == "hello"
  end
  
  specify "rb_str_dup should return a copy of the string" do
    str1 = "hi"
    str2 = @s.dup_string str1
    str1.should == str2
    str1.object_id.should_not == str2.object_id
  end
  
  specify "rb_str_buf_cat should concat C strings to ruby strings" do
    @s.phrase_as_question("Your house is on fire").should == "Your house is on fire?"
  end
end