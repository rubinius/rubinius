shared :regexp_quote do |cmd|
  describe "Regexp.#{cmd}" do
    it "escapes any characters with special meaning in a regular expression" do
      Regexp.send(cmd, '\*?{}.+^[]()-').should == '\\\\\*\?\{\}\.\+\^\[\]\(\)\-'
    end
  end
end
