require File.dirname(__FILE__) + '/../spec_bhelper'

context "RubiniusTarget" do
  setup do
    RubiniusTarget.send(:define_method, :test_fake_name) { nil }
    @target = RubiniusTarget.new("test_fake_name")
  end
  
  specify "should provide example method" do
    @target.should_respond_to :example
  end
end