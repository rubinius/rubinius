require File.dirname(__FILE__) + '/spec_helper'

load_extension('string')

describe "CApiString" do
  before :each do
    @s = CApiStringSpecs.new
  end

  class ValidTostrTest
    def to_str
      "ruby"
    end
  end

  class InvalidTostrTest
    def to_str
      []
    end
  end

  it "rb_str_new should return a new string object" do
    # Hardcoded to pass const char * = "hello"
    @s.say_hello.should == "hello"
  end

  it "rb_str_new2 should return a new string object, figuring out the length itself" do
    # Hardcoded to pass const char * = "hello\0invisible"
    @s.say_hello2.should == "hello"
  end

  it "rb_str_new2 should raise ArgumentError if passed NULL" do
    lambda { @s.rb_str_new2_with_null }.should raise_error(ArgumentError)
  end

  it "rb_str_dup should return a copy of the string" do
    str1 = "hi"
    str2 = @s.dup_string str1
    str1.should == str2
    str1.object_id.should_not == str2.object_id
  end

  it "rb_str_append should append a string to another string" do
    @s.rb_str_append("Hello", " Goodbye").should == "Hello Goodbye"
  end

  it "rb_str_plus should return a new string from two other strings" do
    @s.rb_str_plus("Hello", " Goodbye").should == "Hello Goodbye"
  end

  it "rb_str_buf_cat should concat C strings to ruby strings" do
    @s.phrase_as_question("Your house is on fire").should == "Your house is on fire?"
  end

  it "rb_str_cat should concat C strings to ruby strings" do
    @s.cat_as_question("Your house is on fire").should == "Your house is on fire?"
  end

  it "rb_str_cat2 should concat C strings to ruby strings" do
    @s.cat2_as_question("Your house is on fire").should == "Your house is on fire?"
  end

  it "rb_str_cmp should compare two strings" do
    @s.rb_str_cmp("xxx", "xxxx").should == -1
    @s.rb_str_cmp("xxxx", "xxx").should == 1
    @s.rb_str_cmp("xxx", "yyy").should == -1
    @s.rb_str_cmp("yyy", "xxx").should == 1
    @s.rb_str_cmp("ppp", "ppp").should == 0
  end

  it "rb_str_split should split strings over a splitter" do
    @s.rb_str_split("Hello,Goodbye").should == ["Hello", "Goodbye"]
  end

  it "rb_str2inum should return a string representation as a number" do
    @s.rb_str2inum("10", 10).should == 10
    @s.rb_str2inum("A", 16).should == 10
  end

  it "rb_str2inum should return a string representation as a number" do
    @s.rb_cstr2inum(10).should == 10
    @s.rb_cstr2inum(16).should == 16
  end

  it "rb_str_substr should return a substring" do
    "hello".length.times do |time|
      @s.rb_str_substr("hello", 0, time + 1).should == "hello"[0..time]
    end
  end

  it "RSTRING(str)->ptr should return the string on the object" do
    @s.rb_rstring_see("foo").should == "foo"
  end

  it "Changing RSTRING(str)->ptr should change the string" do
    t = "a"
    @s.rb_rstring_assign_foo(t)
    t.should == "foo"
    $global_rstring_test = "b"
    @s.rb_rstring_assign_global_foobar()
    $global_rstring_test.should == "foobar"
  end

  it "Reducing RSTRING(str)->len should cut the string" do
    t = "12345"
    @s.rb_rstring_set_len(t, 3)
    t.should == "123"
  end

  it "Changing RSTRING(str)->ptr and calling upcase! should change the string and upcase it" do
    t = ""
    @s.rb_rstring_assign_foo_and_upcase(t)
    t.should == "FOO"
  end

  it "rb_str_to_str should try to coerce to String, otherwise raise a TypeError" do
    @s.rb_str_to_str("foo").should == "foo"
    @s.rb_str_to_str(ValidTostrTest.new).should == "ruby"
    lambda { @s.rb_str_to_str(0) }.should raise_error(TypeError)
    lambda { @s.rb_str_to_str(InvalidTostrTest.new) }.should raise_error(TypeError)
  end
end
