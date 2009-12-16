require File.dirname(__FILE__) + '/../spec_helper'

describe "The catch keyword" do
  ruby_version_is "" ... "1.9" do
    it "only allows symbols and strings" do
      lambda { catch(:foo) {} }.should_not raise_error
      lambda { catch("foo") {} }.should_not raise_error
      lambda { catch(1) {} }.should raise_error(ArgumentError)
      lambda { catch(Object.new) {} }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do
    it "allows any object" do
      lambda { catch(:foo) {} }.should_not raise_error
      lambda { catch("foo") {} }.should_not raise_error
      lambda { catch(1) {} }.should_not raise_error
      lambda { catch(Object.new) {} }.should_not raise_error
    end
  end

  it "returns the last value of the block if it nothing is thrown" do
    catch(:exit) do
      :noexit
    end.should == :noexit
  end

  ruby_version_is "" ... "1.9" do
    it "matches strings as symbols" do
      lambda { catch("exit") { throw :exit } }.should_not raise_error
    end

    it "matches strings with strings that contain the same characters" do
      lambda { catch("exit") { throw "exit" } }.should_not raise_error
    end
  end

  ruby_version_is "1.9" do
    it "does not match objects that are not exactly the same" do
      lambda { catch("exit") { throw :exit } }.should raise_error(ArgumentError)
      lambda { catch("exit") { throw "exit" } }.should raise_error(ArgumentError)
    end

    it "catches objects that are exactly the same" do
      lambda { catch(:exit) { throw :exit } }.should_not raise_error
      lambda { exit = "exit"; catch(exit) { throw exit } }.should_not raise_error
    end
  end

  it "requires a block" do
    lambda { catch :foo }.should raise_error(LocalJumpError)
  end

  it "supports nesting" do
    i = []
    catch(:exita) do
      i << :a
      catch(:exitb) do
        i << :b
        throw :exita
        i << :after_throw
      end
      i << :b_exit
    end
    i << :a_exit

    i.should == [:a,:b,:a_exit]
  end

  it "supports nesting with the same name" do
    i = []
    catch(:exit) do
      i << :a
      catch(:exit) do
        i << :b
        throw :exit,:msg
      end.should == :msg
      i << :b_exit
    end.should == [:a,:b,:b_exit]
    i << :a_exit

    i.should == [:a,:b,:b_exit,:a_exit]
  end
end
