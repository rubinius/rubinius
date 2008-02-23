shared :regexp_quote do |cmd|
  describe "Regexp.#{cmd}" do
    it "escapes any characters with special meaning in a regular expression" do
      Regexp.send(cmd, '\*?{}.+^[]()- ').should == '\\\\\*\?\{\}\.\+\^\[\]\(\)\-\\ '
      Regexp.send(cmd, "\*?{}.+^[]()- ").should == '\\*\\?\\{\\}\\.\\+\\^\\[\\]\\(\\)\\-\\ '
      Regexp.send(cmd, '\n\r\f\t').should == '\\\\n\\\\r\\\\f\\\\t'
      Regexp.send(cmd, "\n\r\f\t").should == '\\n\\r\\f\\t'
    end
  end
end
