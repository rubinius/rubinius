require File.expand_path('../../../spec_helper', __FILE__)

# These specs are not intended to be a complete example of the
# strings recognized by the String#count algorithm. See also
# the String#count specs.
describe "String#count_table" do
  it "returns a String of 256 1's if not passed a string" do
    t = "".count_table
    t.size.should == 256
    t.each_byte { |b| b.should == 1 }
  end

  it "returns a String of 256 0's if passed an empty string" do
    t = "".count_table ""
    t.each_byte { |b| b.should == 0 }
  end

  it "returns a String with entries corresponding to characters set to 1" do
    t = "".count_table "-\\|*\030 abcde"
    256.times { |i| t[i].should == ("-|*\030 abcde".include?(i.chr) ? 1 : 0) }
  end

  it "returns a String with entries corresponding to sequences set to 1" do
    t = "".count_table "-ab-g\\d-abhi--A\\"
    256.times { |i|
      t[i].should == ("-abcdefgbhA\\".include?(i.chr) ? 1 : 0)
    }
  end

  it "returns a String with entries corresponding included and excluded characters" do
    t = "".count_table "ab", "^bc"
    256.times { |i| t[i].should == (i == ?a ? 1 : 0) }
  end

  it "returns a String with entries set to false for '^abc-e' inverse match strings" do
    t = "".count_table "^ab-g\\d-abhi--A\\"
    256.times { |i|
      t[i].should == ("abcdefgbhA\\".include?(i.chr) ? 0 : 1)
    }
  end
end
