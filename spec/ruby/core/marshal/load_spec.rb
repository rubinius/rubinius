require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/marshal_data'

describe "Marshal::load" do
  it "loads a extended_struct having fields with same objects" do
    s = 'hi'
    obj = Struct.new("Ure2", :a, :b).new.extend(Meths)
    obj.a = [:a, s]; obj.b = [:Meths, s]

    Marshal.load("\004\be:\nMethsS:\021Struct::Ure2\a:\006a[\a;\a\"\ahi:\006b[\a;\000@\a").should ==
      obj
  end

  it "loads a string having ivar with ref to self" do
    obj = 'hi'
    obj.instance_variable_set(:@self, obj)
    Marshal.load("\004\bI\"\ahi\006:\n@self@\000").should == obj
  end

  it "loads an extended_user_hash with a parameter to initialize" do
    obj = UserHashInitParams.new(:abc).extend(Meths)

    new_obj = Marshal.load "\004\bIe:\nMethsC:\027UserHashInitParams{\000\006:\a@a:\babc"

    new_obj.should == obj
    new_obj_metaclass_ancestors = class << new_obj; ancestors; end
    new_obj_metaclass_ancestors[0].should == Meths
    new_obj_metaclass_ancestors[1].should == UserHashInitParams
  end

  it "loads a user-marshaled extended object" do
    obj = UserMarshal.new.extend(Meths)

    new_obj = Marshal.load "\004\bU:\020UserMarshal\"\nstuff"

    new_obj.should == obj
    new_obj_metaclass_ancestors = class << new_obj; ancestors; end
    new_obj_metaclass_ancestors.first.should == UserMarshal
  end

  it "loads a user_object" do
    obj = UserObject.new
    Marshal.load("\004\bo:\017UserObject\000").class.should == UserObject
  end

  it "loads a object" do
    Marshal.load("\004\bo:\vObject\000").class.should == Object
  end

  it "loads an extended Object" do
    obj = Object.new.extend(Meths)

    new_obj = Marshal.load "\004\be:\nMethso:\vObject\000"

    new_obj.class.should == obj.class
    new_obj_metaclass_ancestors = class << new_obj; ancestors; end
    new_obj_metaclass_ancestors.first(2).should == [Meths, Object]
  end

  it "loads a object having ivar" do
    s = 'hi'
    arr = [:so, :so, s, s]
    obj = Object.new
    obj.instance_variable_set :@str, arr

    new_obj = Marshal.load "\004\bo:\vObject\006:\t@str[\t:\aso;\a\"\ahi@\a"
    new_str = new_obj.instance_variable_get :@str

    new_str.should == arr
  end

  it "loads an extended Regexp" do
    obj = /[a-z]/.extend(Meths, MethsMore)
    new_obj = Marshal.load "\004\be:\nMethse:\016MethsMore/\n[a-z]\000"

    new_obj.should == obj
    new_obj_metaclass_ancestors = class << new_obj; ancestors; end
    new_obj_metaclass_ancestors.first(3).should ==
      [Meths, MethsMore, Regexp]
  end

  it "loads a extended_user_regexp having ivar" do
    obj = UserRegexp.new('').extend(Meths)
    obj.instance_variable_set(:@noise, 'much')

    new_obj = Marshal.load "\004\bIe:\nMethsC:\017UserRegexp/\000\000\006:\v@noise\"\tmuch"

    new_obj.should == obj
    new_obj.instance_variable_get(:@noise).should == 'much'
    new_obj_metaclass_ancestors = class << new_obj; ancestors; end
    new_obj_metaclass_ancestors.first(3).should ==
      [Meths, UserRegexp, Regexp]
  end

  it "loads a Float NaN" do
    obj = 0.0 / 0.0
    Marshal.load("\004\bf\bnan").to_s.should == obj.to_s
  end

  it "loads a Float 1.3" do
    Marshal.load("\004\bf\v1.3\000\314\315").should == 1.3
  end

  it "loads a Float -5.1867345e-22" do
    obj = -5.1867345e-22
    Marshal.load("\004\bf\037-5.1867345000000008e-22\000\203_").should be_close(obj, 1e-30)
  end

  it "loads a Float 1.1867345e+22" do
    obj = 1.1867345e+22
    Marshal.load("\004\bf\0361.1867344999999999e+22\000\344@").should ==
      obj
  end

  ruby_version_is "1.9" do
    it "returns the value of the proc when called with a proc" do
      Marshal.load(Marshal.dump([1,2]), proc { [3,4] }).should ==  [3,4]
    end
  end

  ruby_version_is ""..."1.9" do
    it "ignores the value of the proc when called with a proc" do
      Marshal.load(Marshal.dump([1,2]), proc { [3,4] }).should ==  [1,2]
    end
  end

  ruby_version_is "1.9" do
    it "calls the proc for recursively visited data" do
      a = [1]
      a << a
      ret = []
      Marshal.load(Marshal.dump(a), proc { |arg| ret << arg; arg })
      ret.first.should == 1
      ret[1].should == [1,a]
      ret[2].should == a
      ret.size.should == 3
    end
  end

  ruby_version_is ""..."1.9" do
    it "doesn't call the proc for recursively visited data" do
      a = [1]
      a << a
      ret = []
      Marshal.load(Marshal.dump(a), proc { |arg| ret << arg })
      ret.first.should == 1
      ret.size.should == 2
    end
  end

  ruby_version_is ""..."1.9" do
    it "loads a array containing objects having _dump method, and with proc" do
      arr = []
      proc = Proc.new { |o| arr << o }
      o1 = UserDefined.new; 
      o2 = UserDefinedWithIvar.new
      obj = [o1, o2, o1, o2]

      Marshal.load "\004\b[\tu:\020UserDefined\022\004\b[\a\"\nstuff@\006u:\030UserDefinedWithIvar5\004\b[\bI\"\nstuff\006:\t@foo:\030UserDefinedWithIvar\"\tmore@\a@\006@\a", proc

      arr.should == [o1, o2, obj]
    end
  end

  ruby_version_is "1.9" do
    it "loads a array containing objects having _dump method, and with proc" do
      arr = []
      myproc = Proc.new { |o| arr << o; o }
      o1 = UserDefined.new; 
      o2 = UserDefinedWithIvar.new
      obj = [o1, o2, o1, o2]

      Marshal.load "\x04\b[\tu:\x10UserDefined\x18\x04\b[\aI\"\nstuff\x06:\x06EF@\x06u:\x18UserDefinedWithIvar>\x04\b[\bI\"\nstuff\a:\x06EF:\t@foo:\x18UserDefinedWithIvarI\"\tmore\x06;\x00F@\a@\x06@\a", myproc

      arr.should == [o1, o2, o1, o2, obj]
    end
  end

  ruby_version_is ""..."1.9" do
    it "loads an array containing objects having marshal_dump method, and with proc" do
      arr = []
      proc = Proc.new { |o| arr << o }
      o1 = UserMarshal.new
      o2 = UserMarshalWithIvar.new
      obj = [o1, o2, o1, o2]

      Marshal.load "\004\b[\tU:\020UserMarshal\"\nstuffU:\030UserMarshalWithIvar[\006\"\fmy data@\006@\b", proc

      arr.should == ['stuff', o1, 'my data', ['my data'], o2, obj]
    end
  end

  ruby_version_is "1.9" do
    it "loads an array containing objects having marshal_dump method, and with proc" do
      arr = []
      proc = Proc.new { |o| arr << o; o }
      o1 = UserMarshal.new
      o2 = UserMarshalWithIvar.new
      obj = [o1, o2, o1, o2]

      Marshal.load "\004\b[\tU:\020UserMarshal\"\nstuffU:\030UserMarshalWithIvar[\006\"\fmy data@\006@\b", proc

      arr.should == ['stuff', o1, 'my data', ['my data'], o2, o1, o2, obj]
    end
  end

  ruby_version_is ""..."1.9" do
    it "loads an Array with proc" do
      arr = []
      s = 'hi'
      s.instance_variable_set(:@foo, 5)
      st = Struct.new("Brittle", :a).new
      st.instance_variable_set(:@clue, 'none')
      st.a = 0.0
      h = Hash.new('def')
      h['nine'] = 9
      a = [:a, :b, :c]
      a.instance_variable_set(:@two, 2)
      obj = [s, 10, s, s, st, h, a]
      obj.instance_variable_set(:@zoo, 'ant')
      proc = Proc.new { |o| arr << o }

      new_obj = Marshal.load "\004\bI[\fI\"\ahi\006:\t@fooi\ni\017@\006@\006IS:\024Struct::Brittle\006:\006af\0060\006:\n@clue\"\tnone}\006\"\tninei\016\"\bdefI[\b;\a:\006b:\006c\006:\t@twoi\a\006:\t@zoo\"\bant", proc

      new_obj.should == obj
      new_obj.instance_variable_get(:@zoo).should == 'ant'

      arr.should ==
        [5, s, 10, 0.0, 'none', st, 'nine', 9, 'def', h, :b, :c, 2, a, 'ant', obj]
    end
  end

  ruby_version_is "1.9" do
    it "loads an Array with proc" do
      arr = []
      s = 'hi'
      s.instance_variable_set(:@foo, 5)
      st = Struct.new("Brittle", :a).new
      st.instance_variable_set(:@clue, 'none')
      st.a = 0.0
      h = Hash.new('def')
      h['nine'] = 9
      a = [:a, :b, :c]
      a.instance_variable_set(:@two, 2)
      obj = [s, 10, s, s, st, a]
      obj.instance_variable_set(:@zoo, 'ant')
      proc = Proc.new { |o| arr << o; o}

      Marshal.load("\x04\bI[\vI\"\ahi\a:\x06EF:\t@fooi\ni\x0F@\x06@\x06IS:\x14Struct::Brittle\x06:\x06af\x060\x06:\n@clueI\"\tnone\x06;\x00FI[\b;\b:\x06b:\x06c\x06:\t@twoi\a\x06:\t@zooI\"\bant\x06;\x00F", proc)

      arr.should == ["hi", false, 5, 10, "hi", "hi", 0.0, st, "none", false, 
        :b, :c, a, 2, ["hi", 10, "hi", "hi", st, [:a, :b, :c]], "ant", false]
    end
  end

  it "loads a array containing the same objects" do
    s = 'oh'; b = 'hi'; r = //; d = [b, :no, s, :go]; c = String; f = 1.0
    o1 = UserMarshalWithIvar.new; o2 = UserMarshal.new
    obj = [:so, 'hello', 100, :so, :so, d, :so, o2, :so, :no, o2,
           :go, c, nil, Struct::Pyramid.new, f, :go, :no, s, b, r,
           :so, 'huh', o1, true, b, b, 99, r, b, s, :so, f, c, :no, o1, d]

    Marshal.load("\004\b[*:\aso\"\nhelloii;\000;\000[\t\"\ahi:\ano\"\aoh:\ago;\000U:\020UserMarshal\"\nstuff;\000;\006@\n;\ac\vString0S:\024Struct::Pyramid\000f\0061;\a;\006@\t@\b/\000\000;\000\"\bhuhU:\030UserMarshalWithIvar[\006\"\fmy dataT@\b@\bih@\017@\b@\t;\000@\016@\f;\006@\021@\a").should ==
      obj
  end

  it "loads an array having ivar" do
    s = 'well'
    s.instance_variable_set(:@foo, 10)
    obj = ['5', s, 'hi'].extend(Meths, MethsMore)
    obj.instance_variable_set(:@mix, s)
    Marshal.load("\004\bI[\b\"\0065I\"\twell\006:\t@fooi\017\"\ahi\006:\t@mix@\a").should ==
      obj
  end

  it "loads a struct having ivar" do
    obj = Struct.new("Thick").new
    obj.instance_variable_set(:@foo, 5)
    Marshal.load("\004\bIS:\022Struct::Thick\000\006:\t@fooi\n").should ==
      obj
  end

  it "loads a struct having fields" do
    obj = Struct.new("Ure1", :a, :b).new
    Marshal.load("\004\bS:\021Struct::Ure1\a:\006a0:\006b0").should ==
      obj
  end

  it "raises a TypeError with bad Marshal version" do
    marshal_data = '\xff\xff'
    marshal_data[0] = (Marshal::MAJOR_VERSION).chr
    marshal_data[1] = (Marshal::MINOR_VERSION + 1).chr

    lambda { Marshal.load marshal_data }.should raise_error(TypeError)

    marshal_data = '\xff\xff'
    marshal_data[0] = (Marshal::MAJOR_VERSION - 1).chr
    marshal_data[1] = (Marshal::MINOR_VERSION).chr

    lambda { Marshal.load marshal_data }.should raise_error(TypeError)
  end
  
  it "raises EOFError on loading an empty file" do
    temp_file = tmp("marshal.rubyspec.tmp.#{Process.pid}")
    file = File.new(temp_file, "w+")
    begin
      lambda { Marshal.load(file) }.should raise_error(EOFError)
    ensure
      file.close
      File.unlink(temp_file)
    end
  end

  MarshalSpec::DATA.each do |description, (object, marshal, attributes)|
    it "loads a #{description}" do
      Marshal.load(marshal).should == object
    end
  end
end
