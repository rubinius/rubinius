
describe "Object#metaclass" do
  it "returns the object's metaclass" do
    foo = "foo"
    foo.instance_eval "class << self; def meta_test_method; 5; end; end"
    foo.respond_to?(:meta_test_method).should == true
    should_raise(NameError) { "hello".metaclass.method(:meta_test_method) }
  end
end
