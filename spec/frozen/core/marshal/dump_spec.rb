require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/marshal_data'

mv = [Marshal::MAJOR_VERSION].pack 'C'
nv = [Marshal::MINOR_VERSION].pack 'C'

class UserDefinedBad
  def _dump(depth); 10; end
end

# TODO: these need to be reviewed and cleaned up
describe "Marshal.dump" do
  it "raises an ArgumentError when the recursion limit is exceeded" do
    lambda { Marshal.dump([], 1) }.should_not raise_error(ArgumentError)
    lambda { Marshal.dump([[]], 2) }.should_not raise_error(ArgumentError)
    lambda { Marshal.dump([[[]]], 3) }.should_not raise_error(ArgumentError)

    h = {'one' => {'two' => {'three' => 0}}}
    lambda { Marshal.dump(h, 3) }.should raise_error(ArgumentError)
    lambda { Marshal.dump([h], 4) }.should raise_error(ArgumentError)
    lambda { Marshal.dump({}, 1) }.should_not raise_error(ArgumentError)
    lambda { Marshal.dump(h, 4) }.should_not raise_error(ArgumentError)

    lambda { Marshal.dump([], 0)     }.should raise_error(ArgumentError)
    lambda { Marshal.dump([[[]]], 1) }.should raise_error(ArgumentError)
  end

  it "ignores the recursion limit if the limit is negative" do
    Marshal.dump([], -1).should == "\004\b[\000"
    Marshal.dump([[]], -1).should == "\004\b[\006[\000"
    Marshal.dump([[[]]], -1).should == "\004\b[\006[\006[\000"
  end

  it "writes the serialized data to the IO-Object" do
    (obj = mock('test')).should_receive(:write).at_least(1)
    Marshal.dump("test", obj)
  end

  it "returns the IO-Object" do
    (obj = mock('test')).should_receive(:write).at_least(1)
    Marshal.dump("test", obj).should == obj
  end

  it "raises an Error when the IO-Object does not respond to #write" do
    obj = mock('test')
    lambda { Marshal.dump("test", obj) }.should raise_error(TypeError)
  end

  it "raises an ArgumentError when given more than three arguments" do
    lambda { Marshal.dump(nil, nil, nil, nil) }.should raise_error(ArgumentError)
  end

  it "raises an Error when trying to dump an anonymous class/module" do
    klass = Class.new
    mod = Module.new

    lambda { Marshal.dump(klass) }.should raise_error(TypeError)
    lambda { Marshal.dump(mod)   }.should raise_error(TypeError)
  end

  it "raises a TypeError if _dump returns a non-string" do
    lambda { Marshal.dump(UserDefinedBad.new) }.should raise_error(TypeError)
  end

  it "dumps an Object" do
    Marshal.dump(Object.new).should == "#{mv+nv}o:\x0BObject\x00"
  end

  it "dumps an extended_object" do
    Marshal.dump(Object.new.extend(Meths)).should == "#{mv+nv}e:\x0AMethso:\x0BObject\x00"
  end

  ruby_version_is ""..."1.9" do
    it "dumps an object having ivar" do
      s = 'hi'
      obj = Object.new
      obj.instance_variable_set(:@str, [:so, :so, s, s])
      Marshal.dump(obj).should == "#{mv+nv}o:\x0BObject\x06:\x09@str[\x09:\x07so;\x07\"\x07hi@\x07"
    end
  end

  ruby_version_is "1.9" do
    it "dumps an object having ivar" do
      s = 'hi'
      obj = Object.new
      obj.instance_variable_set(:@str, [:so, :so, s, s])
      Marshal.dump(obj).should == 
        "#{mv+nv}o:\vObject\x06:\t@str[\t:\aso;\aI\"\ahi\x06:\x06EF@\a"
    end
  end

  ruby_version_is ""..."1.9" do
    it "dumps an extended_user_regexp having ivar" do
      r = UserRegexp.new('').extend(Meths)
      r.instance_variable_set(:@noise, 'much')
      Marshal.dump(r).should == "#{mv+nv}Ie:\x0AMethsC:\x0FUserRegexp/\x00\x00\x06:\x0B@noise\"\x09much"
    end
  end

  ruby_version_is "1.9" do
    it "dumps an extended_user_regexp having ivar" do
      r = UserRegexp.new('').extend(Meths)
      r.instance_variable_set(:@noise, 'much')
      Marshal.dump(r).should == 
        "#{mv+nv}Ie:\nMethsC:\x0FUserRegexp/\x00\x00\a:\x06EF:\v@noiseI\"\tmuch\x06;\aF"
    end
  end

  it "raises a TypeError with hash having default proc" do
    lambda { Marshal.dump(Hash.new {}) }.should raise_error(TypeError)
  end

  it "raises a TypeError if marshalling a Method instance" do
    lambda { Marshal.dump(Marshal.method(:dump)) }.should raise_error(TypeError)
  end

  it "raises a TypeError if marshalling a Proc" do
    lambda { Marshal.dump(proc {}) }.should raise_error(TypeError)
  end

  it "raises a TypeError if dumping a IO/File instance" do
    lambda { Marshal.dump(STDIN) }.should raise_error(TypeError)
    lambda { File.open(__FILE__) { |f| Marshal.dump(f) } }.should raise_error(TypeError)
  end

  it "raises a TypeError if dumping a MatchData instance" do
    lambda { "foo" =~ /(.)/; Marshal.dump($~) }.should raise_error(TypeError)
  end

  ruby_version_is ""..."1.9" do
    it "dumps an extended_user_hash_default" do
      h = UserHash.new(:Meths).extend(Meths)
      h['three'] = 3
      Marshal.dump(h).should == "#{mv+nv}e:\x0AMethsC:\x0DUserHash}\x06\"\x0Athreei\x08;\x00"
    end
  end

  ruby_version_is "1.9" do
    it "dumps an extended_user_hash_default" do
      h = UserHash.new(:Meths).extend(Meths)
      h['three'] = 3
      Marshal.dump(h).should == 
        "\x04\be:\nMethsC:\rUserHash}\x06I\"\nthree\x06:\x06EFi\b;\x00"
    end
  end

  ruby_version_is ""..."1.9" do
    it "dumps an extended_user_hash with a parameter to initialize" do
      h = UserHashInitParams.new(:abc).extend(Meths)
      h['three'] = 3
      Marshal.dump(h).should == "\004\bIe:\nMethsC:\027UserHashInitParams{\006\"\nthreei\b\006:\a@a:\babc"
    end
  end

  ruby_version_is "1.9" do
    it "dumps an extended_user_hash with a parameter to initialize" do
      h = UserHashInitParams.new(:abc).extend(Meths)
      h['three'] = 3
      Marshal.dump(h).should == 
        "\x04\bIe:\nMethsC:\x17UserHashInitParams{\x06I\"\nthree\x06:\x06EFi\b\x06:\a@a:\babc"
    end
  end

  ruby_version_is ""..."1.9" do
    it "dumps an array containing objects having _dump method" do
      o1 = UserDefined.new
      o2 = UserDefinedWithIvar.new
      a = [o1, o2, o1, o2]
      Marshal.dump(a).should ==
        "#{mv+nv}[\tu:\020UserDefined\022\004\b[\a\"\nstuff@\006u:\030UserDefinedWithIvar5\004\b[\bI\"\nstuff\006:\t@foo:\030UserDefinedWithIvar\"\tmore@\a@\006@\a"
    end
  end

  ruby_version_is "1.9" do
    it "dumps an array containing objects having _dump method" do
      o1 = UserDefined.new
      o2 = UserDefinedWithIvar.new
      a = [o1, o2, o1, o2]
      Marshal.dump(a).should ==
        "#{mv+nv}[\tu:\x10UserDefined\x18\x04\b[\aI\"\nstuff\x06:\x06EF@\x06u:\x18UserDefinedWithIvar>\x04\b[\bI\"\nstuff\a:\x06EF:\t@foo:\x18UserDefinedWithIvarI\"\tmore\x06;\x00F@\a@\x06@\a"
    end
  end

  ruby_version_is ""..."1.9" do
    it "dumps an array containing objects having marshal_dump method" do
      o1 = UserMarshal.new
      o2 = UserMarshalWithIvar.new
      a = [o1, o2, o1, o2]
      Marshal.dump(a).should ==
        "#{mv+nv}[\tU:\020UserMarshal\"\nstuffU:\030UserMarshalWithIvar[\006\"\fmy data@\006@\b"
    end
  end

  ruby_version_is "1.9" do
    it "dumps an array containing objects having marshal_dump method" do
      o1 = UserMarshal.new
      o2 = UserMarshalWithIvar.new
      a = [o1, o2, o1, o2]
      Marshal.dump(a).should ==
        "#{mv+nv}[\tU:\x10UserMarshalI\"\nstuff\x06:\x06EFU:\x18UserMarshalWithIvar[\x06I\"\fmy data\x06;\x06F@\x06@\b"
    end
  end

  ruby_version_is ""..."1.9" do
    it "dumps an array containing the same objects" do
      s = 'oh'; b = 'hi'; r = //; d = [b, :no, s, :go]; c = String
      a = [:so, 'hello', 100, :so, :so, d, :so, :so, :no, :go, c, nil,
            :go, :no, s, b, r, :so, 'huh', true, b, b, 99, r, b, s, :so, c, :no, d]
      Marshal.dump(a).should ==
        "#{mv+nv}[\x23:\x07so\"\x0Ahelloi\x69;\x00;\x00[\x09\"\x07hi:\x07no\"\x07oh:\x07go;\x00;\x00;\x06;\x07c\x0BString0;\x07;\x06@\x09@\x08/\x00\x00;\x00\"\x08huhT@\x08@\x08i\x68@\x0B@\x08@\x09;\x00@\x0A;\x06@\x07"
    end
  end

  ruby_version_is "1.9" do
    it "dumps an array containing the same objects" do
      s = 'oh'; b = 'hi'; r = //; d = [b, :no, s, :go]; c = String
      a = [:so, 'hello', 100, :so, :so, d, :so, :so, :no, :go, c, nil,
            :go, :no, s, b, r, :so, 'huh', true, b, b, 99, r, b, s, :so, c, :no, d]
      Marshal.dump(a).should ==
        "#{mv+nv}[#:\asoI\"\nhello\x06:\x06EFii;\x00;\x00[\tI\"\ahi\x06;\x06F:\anoI\"\aoh\x06;\x06F:\ago;\x00;\x00;\a;\bc\vString0;\b;\a@\t@\bI/\x00\x00\x06;\x06F;\x00I\"\bhuh\x06;\x06FT@\b@\bih@\v@\b@\t;\x00@\n;\a@\a"
    end
  end

  ruby_version_is ""..."1.9" do
    it "dumps an extended_array having ivar" do
      s = 'well'
      s.instance_variable_set(:@foo, 10)
      a = ['5', s, 'hi'].extend(Meths, MethsMore)
      a.instance_variable_set(:@mix, s)
      Marshal.dump(a).should ==
        "#{mv+nv}Ie:\x0AMethse:\x0EMethsMore[\x08\"\x065I\"\x09well\x06:\x09@fooi\x0F\"\x07hi\x06:\x09@mix@\x07"
    end
  end

  ruby_version_is "1.9" do
    it "dumps an extended_array having ivar" do
      s = 'well'
      s.instance_variable_set(:@foo, 10)
      a = ['5', s, 'hi'].extend(Meths, MethsMore)
      a.instance_variable_set(:@mix, s)
      Marshal.dump(a).should ==
        "#{mv+nv}Ie:\nMethse:\x0EMethsMore[\bI\"\x065\x06:\x06EFI\"\twell\a;\aF:\t@fooi\x0FI\"\ahi\x06;\aF\x06:\t@mix@\a"
    end
  end

  it "dumps a struct having ivar" do
    st = Struct.new("Thick").new
    st.instance_variable_set(:@foo, 5)
    Marshal.dump(st).should == "#{mv+nv}IS:\x12Struct::Thick\x00\x06:\x09@fooi\x0A"
  end

  it "dumps a struct having fields" do
    Marshal.dump(Struct.new("Ure1", :a, :b).new).should == "#{mv+nv}S:\x11Struct::Ure1\x07:\x06a0:\x06b0"
  end

  ruby_version_is ""..."1.9" do
    it "dumps an extended_struct having fields with same objects" do
      s = 'hi'
      st = Struct.new("Ure2", :a, :b).new.extend(Meths)
      st.a = [:a, s]; st.b = [:Meths, s]
      Marshal.dump(st).should ==
        "#{mv+nv}e:\x0AMethsS:\x11Struct::Ure2\x07:\x06a[\x07;\x07\"\x07hi:\x06b[\x07;\x00@\x07"
    end
  end

  ruby_version_is "1.9" do
    it "dumps an extended_struct having fields with same objects" do
      s = 'hi'
      st = Struct.new("Ure2", :a, :b).new.extend(Meths)
      st.a = [:a, s]; st.b = [:Meths, s]
      Marshal.dump(st).should ==
        "#{mv+nv}e:\nMethsS:\x11Struct::Ure2\a:\x06a[\a;\aI\"\ahi\x06:\x06EF:\x06b[\a;\x00@\a"
    end
  end
end

describe "Marshal.dump" do
  ruby_version_is ""..."1.9" do
    MarshalSpec::DATA.each do |description, (object, marshal, attributes)|
      it "dumps a #{description}" do
        unless attributes then
          Marshal.dump(object).should == marshal
        else
          # these objects have non-deterministic field order in the
          # marshal stream, so they need a round trip and independent
          # verification.
          object = Marshal.load(Marshal.dump(object))
          attributes.each do |attr, val|
            object.send(attr).should == val
          end
        end
      end
    end
  end

  ruby_version_is "1.9" do
    MarshalSpec::DATA_19.each do |description, (object, marshal, attributes)|
      it "dumps a #{description}" do
        unless attributes then
          Marshal.dump(object).should == marshal
        else
          # these objects have non-deterministic field order in the
          # marshal stream, so they need a round trip and independent
          # verification.
          object = Marshal.load(Marshal.dump(object))
          attributes.each do |attr, val|
            object.send(attr).should == val
          end
        end
      end
    end
  end
end
