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
    256.times { |i| t.getbyte(i).should == ("-|*\030 abcde".include?(i.chr) ? 1 : 0) }
  end

  it "returns a String with entries corresponding to sequences set to 1 without an invalid range" do
    t = "".count_table "-ab-g\\f-hbhi--A\\"
    256.times { |i|
      t.getbyte(i).should == ("-abcdefgfghbhA\\".include?(i.chr) ? 1 : 0)
    }
  end

  ruby_version_is "" ... "1.9" do
    it "returns a String with entries corresponding to sequences set to 1 ignoring an invalid range" do
      t = "".count_table "-ab-g\\d-abhi--A\\"
      256.times { |i|
        t.getbyte(i).should == ("-abcdefgbhA\\".include?(i.chr) ? 1 : 0)
      }
    end
  end

  ruby_version_is "1.9" do
    it "raises an exception for input provided with invalid range" do
      lambda { "".count_table "-ab-g\\d-abhi--A\\" }.should raise_error(ArgumentError)
    end
  end

  it "returns a String with entries corresponding included and excluded characters" do
    t = "".count_table "ab", "^bc"
    256.times { |i| t.getbyte(i).should == (i == 97 ? 1 : 0) }
  end

  it "returns a String with entries set to false for '^abc-e' inverse match strings" do
    t = "".count_table "^ab-g\\f-hbhi--A\\"
    256.times { |i|
      t.getbyte(i).should == ("abcdefgfghbhA\\".include?(i.chr) ? 0 : 1)
    }
  end

  ruby_version_is "" ... "1.9" do
    it "returns a String with entries set to false for '^abc-e' inverse match strings ignoring the invalid range" do
      t = "".count_table "^ab-g\\d-abhi--A\\"
      256.times { |i|
        t.getbyte(i).should == ("abcdefgbhA\\".include?(i.chr) ? 0 : 1)
      }
    end
  end

  ruby_version_is "1.9" do
    it "raises an exception for '^abc-e' inverse match strings with an invalid range" do
      lambda { "".count_table "^ab-g\\d-abhi--A\\" }.should raise_error(ArgumentError)
    end
  end

  describe "input validation" do
    it "raises no exception when input is correct" do
      lambda { "".count_table("abc") }.should_not raise_error
    end

    it "raises no exception when input contains a correct range" do
      lambda { "".count_table("a-c") }.should_not raise_error
    end

    ruby_version_is "1.9" do
      it "raises a proper ArgumentError when input contains a range out of sequence" do
        lambda { "".count_table("h-e") }.should raise_error(ArgumentError, 'invalid range "h-e" in string transliteration')
        lambda { "".count_table("^h-e") }.should raise_error(ArgumentError, 'invalid range "h-e" in string transliteration')
        lambda { "".count_table("abc", "ash-e") }.should raise_error(ArgumentError, 'invalid range "h-e" in string transliteration')
      end
    end
  end
end
