require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel.catch" do
  it "executes its block" do
    block_executed = false
    result = catch :blah do
      block_executed = true
    end
    block_executed.should == true
  end

  it "returns the last expression evaluated if throw was not called" do
    result = catch :blah do
      :normal_return_value
    end
    result.should == :normal_return_value
  end

  it "throws the given name and is caught by matching catch block" do
    bad = false
    catch :blah do
      throw :blah
      bad = true
    end
    bad.should == false
  end

  it "allows a String to be used for the label" do
    name = "hello"
    x = catch(name) { throw name, :fin }
    x.should == :fin
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


  it "can be used even in a method different from where throw is called" do
    class CatchSpecs
      def self.throwing_method
        throw :blah, :thrown_value
      end
      def self.catching_method
        catch :blah do
          throwing_method
        end
      end
    end
    CatchSpecs.catching_method.should == :thrown_value
  end

  it "can be nested" do
    one = two = three = 0
    catch :three do
      catch :two do
        catch :three do
          three = 3
          throw :three
          three = :wrong
        end
        two = 2
        throw :two
        two = :wrong
      end
      one = 1
      throw :three
      one = :wrong
    end
    [one, two, three].should == [1, 2, 3]
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

  ruby_version_is ""..."1.9" do
    it "raises TypeError if the argument is not a Symbol or String" do
      lambda {
        catch(Object.new) {}
      }.should raise_error(TypeError)
    end

    it "raises ArgumentError if called without argument" do
      lambda { catch {} }.should raise_error(ArgumentError)
    end
  end

  ruby_version_is "1.9" do
    it "accepts an object as an argument" do
      lambda {
        catch Object.new do end
      }.should_not raise_error
    end

    it "yields a new, unique object when called without arguments" do
      catch {|obj| obj.should be_an_instance_of(Object) }
    end
  end

  it "raises LocalJumpError if no block is given" do
    lambda { catch :blah }.should raise_error(LocalJumpError)
  end
end

describe "Kernel#catch" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:catch)
  end
end

