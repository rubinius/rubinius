describe :regexp_quote, :shared => true do
  it "escapes any characters with special meaning in a regular expression" do
    Regexp.send(@method, '\*?{}.+^[]()- ').should == '\\\\\*\?\{\}\.\+\^\[\]\(\)\-\\ '
    Regexp.send(@method, "\*?{}.+^[]()- ").should == '\\*\\?\\{\\}\\.\\+\\^\\[\\]\\(\\)\\-\\ '
    Regexp.send(@method, '\n\r\f\t').should == '\\\\n\\\\r\\\\f\\\\t'
    Regexp.send(@method, "\n\r\f\t").should == '\\n\\r\\f\\t'
  end
end
