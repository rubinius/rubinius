require File.dirname(__FILE__) + '/../spec_helper'

context "CType instance method" do
  specify "isspace should return true if self is ASCII whitespace" do
    example do
      @a = []
      "\tt\fi a\r\nt\v".each_byte { |b| @a << b.isspace }
      @a
    end.should == [true, false, true, false, true, false, true, true, false, true]
  end
  
  specify "isupper should return true if self is between A..Z" do
    example do
      @a = []
      "MenTaLguY".each_byte { |b| @a << b.isupper }
      @a
    end.should == [true, false, false, true, false, true, false, false, true]
  end
  
  specify "islower should return true if self is between a..z" do
    example do
      @a = []
      "MenTaLguY".each_byte { |b| @a << b.islower }
      @a
    end.should == [false, true, true, false, true, false, true, true, false]
  end
  
  specify "tolower should return self in range a..z if self is between A..Z, otherwise return self" do
    example do
      @a = []
      "MenTaLguY".each_byte { |b| @a << b.tolower }
      @a
    end.should == [109, 101, 110, 116, 97, 108, 103, 117, 121]
  end
  
  specify "toupper should return a value in the range A..Z if self is between a..z, otherwise return self" do
    example do
      @a = []
      "MenTaLguY".each_byte { |b| @a << b.toupper }
      @a
    end.should == [77, 69, 78, 84, 65, 76, 71, 85, 89]
  end
end
