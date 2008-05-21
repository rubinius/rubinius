shared :string_succ do |cmd|
  describe "String##{cmd}" do
    it "returns an empty string for empty strings" do
      "".send(cmd).should == ""
    end
    
    it "returns the successor by increasing the rightmost alphanumeric (digit => digit, letter => letter with same case)" do
      "abcd".send(cmd).should == "abce"
      "THX1138".send(cmd).should == "THX1139"
      
      "<<koala>>".send(cmd).should == "<<koalb>>"
      "==A??".send(cmd).should == "==B??"
    end
    
    it "increases non-alphanumerics (via ascii rules) if there are no alphanumerics" do
      "***".send(cmd).should == "**+"
      "**`".send(cmd).should == "**a"
    end
    
    it "increases the next best alphanumeric (jumping over non-alphanumerics) if there is a carry" do
      "dz".send(cmd).should == "ea"
      "HZ".send(cmd).should == "IA"
      "49".send(cmd).should == "50"
      
      "izz".send(cmd).should == "jaa"
      "IZZ".send(cmd).should == "JAA"
      "699".send(cmd).should == "700"
      
      "6Z99z99Z".send(cmd).should == "7A00a00A"
      
      "1999zzz".send(cmd).should == "2000aaa"
      "NZ/[]ZZZ9999".send(cmd).should == "OA/[]AAA0000"
    end
  
    it "increases the next best character if there is a carry for non-alphanumerics" do
      "(\xFF".send(cmd).should == ")\x00"
      "`\xFF".send(cmd).should == "a\x00"
      "<\xFF\xFF".send(cmd).should == "=\x00\x00"
    end
  
    it "adds an additional character (just left to the last increased one) if there is a carry and no character left to increase" do
      "z".send(cmd).should == "aa"
      "Z".send(cmd).should == "AA"
      "9".send(cmd).should == "10"
      
      "zz".send(cmd).should == "aaa"
      "ZZ".send(cmd).should == "AAA"
      "99".send(cmd).should == "100"
  
      "9Z99z99Z".send(cmd).should == "10A00a00A"
  
      "ZZZ9999".send(cmd).should == "AAAA0000"
      "/[]ZZZ9999".send(cmd).should == "/[]AAAA0000"
      "Z/[]ZZZ9999".send(cmd).should == "AA/[]AAA0000"
      
      # non-alphanumeric cases
      "\xFF".send(cmd).should == "\x01\x00"
      "\xFF\xFF".send(cmd).should == "\x01\x00\x00"
    end
  
    it "returns subclass instances when called on a subclass" do
      StringSpecs::MyString.new("").send(cmd).class.should == StringSpecs::MyString
      StringSpecs::MyString.new("a").send(cmd).class.should == StringSpecs::MyString
      StringSpecs::MyString.new("z").send(cmd).class.should == StringSpecs::MyString
    end
    
    it "taints the result if self is tainted" do
      ["", "a", "z", "Z", "9", "\xFF", "\xFF\xFF"].each do |s|
        s.taint.send(cmd).tainted?.should == true
      end
    end
  end
end

shared :string_succ_bang do |cmd|
  describe "String##{cmd}" do
    it "is equivalent to succ, but modifies self in place (still returns self)" do
      ["", "abcd", "THX1138"].each do |s|
        r = s.dup.send(cmd)
        s.send(cmd).should equal(s)
        s.should == r
      end
    end
    
    compliant_on :ruby, :jruby do
      it "raises a TypeError if self is frozen" do
        lambda { "".freeze.send(cmd)     }.should raise_error(TypeError)
        lambda { "abcd".freeze.send(cmd) }.should raise_error(TypeError)
      end
    end
  end
end