require File.dirname(__FILE__) + '/../spec_helper'

describe "The throw keyword" do
  it "should abandon processing" do
    i = 0
    catch(:done) do
      loop do
        i += 1
        throw :done if i > 4
      end
      i += 1
    end    
    i.should == 5
  end

  it "should support a second parameter" do
    msg = catch(:exit) do
      throw :exit,:msg
    end
    msg.should == :msg
  end

  it "should use nil as a default second parameter" do
    msg = catch(:exit) do
      throw :exit
    end
    msg.should == nil
  end

  it "should return the last value of catch if it nothing is thrown" do
    catch(:exit) do      
      :noexit
    end.should == :noexit
  end
  
  it "should support nesting" do
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

  it "should support nesting with the same name" do
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

  it "should unwind stack from within a method" do
    def throw_method(handler,val)
      throw handler,val
    end

    catch(:exit) do
      throw_method(:exit,5)
    end.should == 5
  end
  
  it "should raise a name error if outside of scope of a matching catch" do    
    should_raise(NameError) { throw :test,5 }
    should_raise(NameError) { catch(:different) { throw :test,5 } }
  end
end

