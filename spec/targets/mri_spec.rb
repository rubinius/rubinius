require File.dirname(__FILE__) + '/../spec_bhelper'

context "MRITarget" do
  setup do
    MRITarget.send(:define_method, :test_fake_name) { nil }
    # I wish this worked MRITarget.instance_stub!(:test_fake_name).and_return(nil)
    @target = MRITarget.new('test_fake_name')
  end
  
  specify "should provide example method" do
    @target.should_respond_to :example
  end
  
  specify "example should return the value of the block it receives" do
    a = [1, 2, 3]
    example { a }.should_equal? a
  end
end
