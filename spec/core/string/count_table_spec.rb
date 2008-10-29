require File.dirname(__FILE__) + '/../../spec_helper'

# These specs are not intended to be a complete example of the
# strings recognized by the String#count algorithm. See also
# the String#count specs.
describe "String#count_table" do
  it "returns an all true bitmap if passed nothing" do
    table = "".count_table
    table.size.should == 256
    table.each { |value| value.should == true }
  end

  it "returns an all false bitmap if passed an empty string" do
    table = "".count_table ""
    table.each { |value| value.should == false }
  end

  it "returns a bitmap with matched characters set to true" do
    table = "".count_table "-\\|*\030 abcde"
    table.each_with_index do |value, i|
      value.should == "-|*\030 abcde".include?(i.chr)
    end
  end

  it "returns a bitmap with matched character ranges set to true" do
    table = "".count_table "-ab-g\\d-abhi--A\\"
    table.each_with_index do |value, i|
      value.should == "-abcdefgbhA\\".include?(i.chr)
    end
  end

  it "returns a bitmap with excluded characters set to false" do
    table = "".count_table "ab", "^bc"

    table.each_with_index do |value, i|
      value.should == 'a'.include?(i.chr)
    end
  end

  it "returns a bitmap with excluded character ranges set to false" do
    table = "".count_table "^ab-g\\d-abhi--A\\"

    table.each_with_index do |value, i|
      value.should_not == "abcdefgbhA\\".include?(i.chr)
    end
  end
end
