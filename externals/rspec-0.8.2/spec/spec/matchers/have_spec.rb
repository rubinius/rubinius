require File.dirname(__FILE__) + '/../../spec_helper.rb'

module HaveSpecHelper
  def create_collection_owner_with(n)
    owner = Spec::Expectations::Helper::CollectionOwner.new
    (1..n).each do |n|
      owner.add_to_collection_with_length_method(n)
      owner.add_to_collection_with_size_method(n)
    end
    owner
  end
end

context "should have(n).items" do
  include HaveSpecHelper

  specify "should pass if target has a collection of items with n members" do
    owner = create_collection_owner_with(3)
    owner.should have(3).items_in_collection_with_length_method
    owner.should have(3).items_in_collection_with_size_method
  end

  specify "should convert :no to 0" do
    owner = create_collection_owner_with(0)
    owner.should have(:no).items_in_collection_with_length_method
    owner.should have(:no).items_in_collection_with_size_method
  end

  specify "should fail if target has a collection of items with < n members" do
    owner = create_collection_owner_with(3)
    lambda {
      owner.should have(4).items_in_collection_with_length_method
    }.should fail_with("expected 4 items_in_collection_with_length_method, got 3")
    lambda {
      owner.should have(4).items_in_collection_with_size_method
    }.should fail_with("expected 4 items_in_collection_with_size_method, got 3")
  end
  
  specify "should fail if target has a collection of items with > n members" do
    owner = create_collection_owner_with(3)
    lambda {
      owner.should have(2).items_in_collection_with_length_method
    }.should fail_with("expected 2 items_in_collection_with_length_method, got 3")
    lambda {
      owner.should have(2).items_in_collection_with_size_method
    }.should fail_with("expected 2 items_in_collection_with_size_method, got 3")
  end
end

context "should_not have(n).items" do
  include HaveSpecHelper

  specify "should pass if target has a collection of items with < n members" do
    owner = create_collection_owner_with(3)
    owner.should_not have(4).items_in_collection_with_length_method
    owner.should_not have(4).items_in_collection_with_size_method
  end
  
  specify "should pass if target has a collection of items with > n members" do
    owner = create_collection_owner_with(3)
    owner.should_not have(2).items_in_collection_with_length_method
    owner.should_not have(2).items_in_collection_with_size_method
  end

  specify "should fail if target has a collection of items with n members" do
    owner = create_collection_owner_with(3)
    lambda {
      owner.should_not have(3).items_in_collection_with_length_method
    }.should fail_with("expected target not to have 3 items_in_collection_with_length_method, got 3")
    lambda {
      owner.should_not have(3).items_in_collection_with_size_method
      }.should fail_with("expected target not to have 3 items_in_collection_with_size_method, got 3")
  end
end

context "should have_exactly(n).items" do
  include HaveSpecHelper

  specify "should pass if target has a collection of items with n members" do
    owner = create_collection_owner_with(3)
    owner.should have_exactly(3).items_in_collection_with_length_method
    owner.should have_exactly(3).items_in_collection_with_size_method
  end

  specify "should convert :no to 0" do
    owner = create_collection_owner_with(0)
    owner.should have_exactly(:no).items_in_collection_with_length_method
    owner.should have_exactly(:no).items_in_collection_with_size_method
  end

  specify "should fail if target has a collection of items with < n members" do
    owner = create_collection_owner_with(3)
    lambda {
      owner.should have_exactly(4).items_in_collection_with_length_method
    }.should fail_with("expected 4 items_in_collection_with_length_method, got 3")
    lambda {
      owner.should have_exactly(4).items_in_collection_with_size_method
    }.should fail_with("expected 4 items_in_collection_with_size_method, got 3")
  end
  
  specify "should fail if target has a collection of items with > n members" do
    owner = create_collection_owner_with(3)
    lambda {
      owner.should have_exactly(2).items_in_collection_with_length_method
    }.should fail_with("expected 2 items_in_collection_with_length_method, got 3")
    lambda {
      owner.should have_exactly(2).items_in_collection_with_size_method
    }.should fail_with("expected 2 items_in_collection_with_size_method, got 3")
  end
end

context "should have_at_least(n).items" do
  include HaveSpecHelper

  specify "should pass if target has a collection of items with n members" do
    owner = create_collection_owner_with(3)
    owner.should have_at_least(3).items_in_collection_with_length_method
    owner.should have_at_least(3).items_in_collection_with_size_method
  end
  
  specify "should pass if target has a collection of items with > n members" do
    owner = create_collection_owner_with(3)
    owner.should have_at_least(2).items_in_collection_with_length_method
    owner.should have_at_least(2).items_in_collection_with_size_method
  end

  specify "should fail if target has a collection of items with < n members" do
    owner = create_collection_owner_with(3)
    lambda {
      owner.should have_at_least(4).items_in_collection_with_length_method
    }.should fail_with("expected at least 4 items_in_collection_with_length_method, got 3")
    lambda {
      owner.should have_at_least(4).items_in_collection_with_size_method
    }.should fail_with("expected at least 4 items_in_collection_with_size_method, got 3")
  end
  
  specify "should provide educational negative failure messages" do
    #given
    owner = create_collection_owner_with(3)
    length_matcher = have_at_least(3).items_in_collection_with_length_method
    size_matcher = have_at_least(3).items_in_collection_with_size_method
    
    #when
    length_matcher.matches?(owner)
    size_matcher.matches?(owner)
    
    #then
    length_matcher.negative_failure_message.should == <<-EOF
Isn't life confusing enough?
Instead of having to figure out the meaning of this:
  should_not have_at_least(3).items_in_collection_with_length_method
We recommend that you use this instead:
  should have_at_most(2).items_in_collection_with_length_method
EOF

    size_matcher.negative_failure_message.should == <<-EOF
Isn't life confusing enough?
Instead of having to figure out the meaning of this:
  should_not have_at_least(3).items_in_collection_with_size_method
We recommend that you use this instead:
  should have_at_most(2).items_in_collection_with_size_method
EOF
  end
end

context "should have_at_most(n).items" do
  include HaveSpecHelper

  specify "should pass if target has a collection of items with n members" do
    owner = create_collection_owner_with(3)
    owner.should have_at_most(3).items_in_collection_with_length_method
    owner.should have_at_most(3).items_in_collection_with_size_method
  end

  specify "should fail if target has a collection of items with > n members" do
    owner = create_collection_owner_with(3)
    lambda {
      owner.should have_at_most(2).items_in_collection_with_length_method
    }.should fail_with("expected at most 2 items_in_collection_with_length_method, got 3")
    lambda {
      owner.should have_at_most(2).items_in_collection_with_size_method
    }.should fail_with("expected at most 2 items_in_collection_with_size_method, got 3")
  end
  
  specify "should pass if target has a collection of items with < n members" do
    owner = create_collection_owner_with(3)
    owner.should have_at_most(4).items_in_collection_with_length_method
    owner.should have_at_most(4).items_in_collection_with_size_method
  end

  specify "should provide educational negative failure messages" do
    #given
    owner = create_collection_owner_with(3)
    length_matcher = have_at_most(3).items_in_collection_with_length_method
    size_matcher = have_at_most(3).items_in_collection_with_size_method
    
    #when
    length_matcher.matches?(owner)
    size_matcher.matches?(owner)
    
    #then
    length_matcher.negative_failure_message.should == <<-EOF
Isn't life confusing enough?
Instead of having to figure out the meaning of this:
  should_not have_at_most(3).items_in_collection_with_length_method
We recommend that you use this instead:
  should have_at_least(4).items_in_collection_with_length_method
EOF
    
    size_matcher.negative_failure_message.should == <<-EOF
Isn't life confusing enough?
Instead of having to figure out the meaning of this:
  should_not have_at_most(3).items_in_collection_with_size_method
We recommend that you use this instead:
  should have_at_least(4).items_in_collection_with_size_method
EOF
  end
end

context "have(n).items(args, block)" do
  specify "should pass args to target" do
    target = mock("target")
    target.should_receive(:items).with("arg1","arg2").and_return([1,2,3])
    target.should have(3).items("arg1","arg2")
  end

  specify "should pass block to target" do
    target = mock("target")
    block = lambda { 5 }
    target.should_receive(:items).with("arg1","arg2", block).and_return([1,2,3])
    target.should have(3).items("arg1","arg2", block)
  end
end

context "have(n).items where target IS a collection" do
  specify "should reference the number of items IN the collection" do
    [1,2,3].should have(3).items
  end

  specify "should reference the number of items IN the collection" do
    lambda { [1,2,3].should have(7).items }.should_fail_with("expected 7 items, got 3")
  end
end

context "have(n).characters where target IS a String" do
  specify "should pass if the length is correct" do
    "this string".should have(11).characters
  end

  specify "should fail if the length is incorrect" do
    lambda { "this string".should have(12).characters }.should_fail_with("expected 12 characters, got 11")
  end
end

context "have(n).things on an object which is not a collection nor contains one" do
  specify "should fail" do
    lambda { Object.new.should have(2).things }.should_raise(NoMethodError, /undefined method `things' for #<Object:/)
  end
end