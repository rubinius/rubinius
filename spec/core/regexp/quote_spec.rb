require File.dirname(__FILE__) + '/../../spec_helper'

@regexp_quote = shared "Regexp.quote" do |cmd|
  describe "Regexp.#{cmd}" do
    it "escapes any characters with special meaning in a regular expression" do
      Regexp.send(cmd, '\*?{}.+^[]()-').should == '\\\\\*\?\{\}\.\+\^\[\]\(\)\-'
    end
  end
end

describe "Range.quote" do
  it_behaves_like(@regexp_quote, :quote)
end
