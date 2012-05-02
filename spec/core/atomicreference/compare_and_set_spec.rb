require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::AtomicReference#compare_and_set" do
  it "sets to new value if its value is same as old value" do
    initial_value = old_value = 0
    new_value = 1
    atomic_reference = Rubinius::AtomicReference.new(initial_value)
    atomic_reference.compare_and_set(old_value, new_value).should == true
    atomic_reference.value.should == new_value
  end

  it "doesn't set to new value if its value isn't same as old value" do
    initial_value = 0
    old_value = 1
    new_value = 2
    atomic_reference = Rubinius::AtomicReference.new(initial_value)
    atomic_reference.compare_and_set(old_value, new_value).should == false
    atomic_reference.value.should == initial_value
  end
end
