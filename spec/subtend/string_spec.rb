require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_string')
require File.dirname(__FILE__) + '/ext/subtend_string'

describe "SubtendString" do
  setup do
    @s = SubtendString.new
  end
  
  specify "rb_str_new should return a new string object" do
    # Hardcoded to pass const char * = "hello"
    @s.say_hello.should == "hello"
  end

  specify "rb_str_new2 should return a new string object, figuring out the length itself" do
    # Hardcoded to pass const char * = "hello\0invisible"
    @s.say_hello2.should == "hello"
  end

  specify "rb_str_new2 should raise ArgumentError if passed NULL" do
    lambda { @s.rb_str_new2_with_null }.should raise_error(ArgumentError)
  end

  specify "rb_str_dup should return a copy of the string" do
    str1 = "hi"
    str2 = @s.dup_string str1
    str1.should == str2
    str1.object_id.should_not == str2.object_id
  end
  
  specify "rb_str_append should append a string to another string" do
    @s.rb_str_append("Hello", " Goodbye").should == "Hello Goodbye"
  end
  
  specify "rb_str_plus should return a new string from two other strings" do
    @s.rb_str_plus("Hello", " Goodbye").should == "Hello Goodbye"
  end

  specify "rb_str_buf_cat should concat C strings to ruby strings" do
    @s.phrase_as_question("Your house is on fire").should == "Your house is on fire?"
  end
  
  specify "rb_str_cat should concat C strings to ruby strings" do
    @s.cat_as_question("Your house is on fire").should == "Your house is on fire?"
  end
  
  specify "rb_str_cmp should compare two strings" do
    @s.rb_str_cmp("xxx", "xxxx").should == -1
    @s.rb_str_cmp("xxxx", "xxx").should == 1
    @s.rb_str_cmp("xxx", "yyy").should == -1
    @s.rb_str_cmp("yyy", "xxx").should == 1
  end
  
  specify "rb_str_split should split strings over a splitter" do
    @s.rb_str_split("Hello,Goodbye").should == ["Hello", "Goodbye"]
  end
  
  specify "rb_str2inum should return a string representation as a number" do
    @s.rb_str2inum("10", 10).should == 10
    @s.rb_str2inum("A", 16).should == 10
  end

  specify "rb_str2inum should return a string representation as a number" do
    @s.rb_cstr2inum(10).should == 10
    @s.rb_cstr2inum(16).should == 16
  end
  
  specify "rb_str_substr should return a substring" do
    "hello".length.times do |time|
      @s.rb_str_substr("hello", 0, time + 1).should == "hello"[0..time]
    end
  end

end
