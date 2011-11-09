require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is '1.9' do
  describe :string_prepend do
    it "prepends the given argument to self and returns self" do
      str = 'world'
      str.prepend('hello ').should equal(str)
      str.should == "hello world"
    end
  end
end
