require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "should be_predicate" do  
  specify "should pass when actual returns true for :predicate?" do
    actual = stub("actual", :happy? => true)
    actual.should be_happy
  end

  specify "should pass when actual returns true for :predicates? (present tense)" do
    actual = stub("actual", :exists? => true)
    actual.should be_exist
  end

  specify "should fail when actual returns false for :predicate?" do
    actual = stub("actual", :happy? => false)
    lambda {
      actual.should be_happy
    }.should fail_with("expected happy? to return true, got false")
  end
  
  specify "should fail when actual does not respond to :predicate?" do
    lambda {
      Object.new.should be_happy
    }.should raise_error(NameError)
  end
end

context "should_not be_predicate" do
  specify "should pass when actual returns false for :sym?" do
    actual = stub("actual", :happy? => false)
    actual.should_not be_happy
  end
  
  specify "should fail when actual returns true for :sym?" do
    actual = stub("actual", :happy? => true)
    lambda {
      actual.should_not be_happy
    }.should fail_with("expected happy? to return false, got true")
  end

  specify "should fail when actual does not respond to :sym?" do
    lambda {
      Object.new.should_not be_happy
    }.should raise_error(NameError)
  end
end

context "should be_predicate(*args)" do
  specify "should pass when actual returns true for :predicate?(*args)" do
    actual = mock("actual")
    actual.should_receive(:older_than?).with(3).and_return(true)
    actual.should be_older_than(3)
  end

  specify "should fail when actual returns false for :predicate?(*args)" do
    actual = mock("actual")
    actual.should_receive(:older_than?).with(3).and_return(false)
    lambda {
      actual.should be_older_than(3)
    }.should fail_with("expected older_than?(3) to return true, got false")
  end
  
  specify "should fail when actual does not respond to :predicate?" do
    lambda {
      Object.new.should be_older_than(3)
    }.should raise_error(NameError)
  end
end

context "should_not be_predicate(*args)" do
  specify "should pass when actual returns false for :predicate?(*args)" do
    actual = mock("actual")
    actual.should_receive(:older_than?).with(3).and_return(false)
    actual.should_not be_older_than(3)
  end
  
  specify "should fail when actual returns true for :predicate?(*args)" do
    actual = mock("actual")
    actual.should_receive(:older_than?).with(3).and_return(true)
    lambda {
      actual.should_not be_older_than(3)
    }.should fail_with("expected older_than?(3) to return false, got true")
  end

  specify "should fail when actual does not respond to :predicate?" do
    lambda {
      Object.new.should_not be_older_than(3)
    }.should raise_error(NameError)
  end
end

context "should be_true" do
  specify "should pass when actual equal(true)" do
    true.should be_true
  end

  specify "should fail when actual equal(false)" do
    lambda {
      false.should be_true
    }.should fail_with("expected true, got false")
  end
end

context "should be_false" do
  specify "should pass when actual equal(false)" do
    false.should be_false
  end

  specify "should fail when actual equal(true)" do
    lambda {
      true.should be_false
    }.should fail_with("expected false, got true")
  end
end

context "should be_nil" do
  specify "should pass when actual is nil" do
    nil.should be_nil
  end

  specify "should fail when actual is not nil" do
    lambda {
      :not_nil.should be_nil
    }.should fail_with("expected nil, got :not_nil")
  end
end

context "should_not be_nil" do
  specify "should pass when actual is not nil" do
    :not_nil.should_not be_nil
  end

  specify "should fail when actual is nil" do
    lambda {
      nil.should_not be_nil
    }.should fail_with("expected not nil, got nil")
  end
end

context "should be <" do
  specify "should pass when < operator returns true" do
    3.should be < 4
  end

  specify "should fail when < operator returns false" do
    lambda { 3.should be < 3 }.should fail_with("expected < 3, got 3")
  end
end

context "should be <=" do
  specify "should pass when <= operator returns true" do
    3.should be <= 4
    4.should be <= 4
  end

  specify "should fail when <= operator returns false" do
    lambda { 3.should be <= 2 }.should fail_with("expected <= 2, got 3")
  end
end

context "should be >=" do
  specify "should pass when >= operator returns true" do
    4.should be >= 4
    5.should be >= 4
  end

  specify "should fail when >= operator returns false" do
    lambda { 3.should be >= 4 }.should fail_with("expected >= 4, got 3")
  end
end

context "should be >" do
  specify "should pass when > operator returns true" do
    5.should be > 4
  end

  specify "should fail when > operator returns false" do
    lambda { 3.should be > 4 }.should fail_with("expected > 4, got 3")
  end
end


