require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#exit" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:exit)
  end
end

describe "Kernel#exit!" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:exit!)
  end
end

describe "Kernel.exit" do
  it "raises a SystemExit with status 0" do
    lambda { exit }.should raise_error(SystemExit) { |e|
      e.status.should == 0
    }
  end

  it "raises a SystemExit with the specified status" do
    [-2**16, -2**8, -8, -1, 0, 1 , 8, 2**8, 2**16].each do |value|
      lambda { exit(value) }.should raise_error(SystemExit) { |e|
        e.status.should == value
      }
    end
  end

  it "raises a SystemExit with the specified boolean status" do
    { true => 0, false => 1 }.each do |value, status|
      lambda { exit(value) }.should raise_error(SystemExit) { |e|
        e.status.should == status
      }
    end
  end

  it "tries to convert the passed argument to an Integer using #to_int" do
    obj = mock('5')
    obj.should_receive(:to_int).and_return(5)
    lambda { exit(obj) }.should raise_error(SystemExit) { |e|
      e.status.should == 5
    }
  end

  it "converts the passed Float argument to an Integer" do
    { -2.2 => -2, -0.1 => 0, 5.5 => 5, 827.999 => 827 }.each do |value, status|
      lambda { exit(value) }.should raise_error(SystemExit) { |e|
        e.status.should == status
      }
    end
  end

  it "raises TypeError if can't convert the argument to an Integer" do
    lambda { exit(Object.new) }.should raise_error(TypeError)
    lambda { exit('0') }.should raise_error(TypeError)
    lambda { exit([0]) }.should raise_error(TypeError)
    lambda { exit(nil) }.should raise_error(TypeError)
  end
end

describe "Kernel.exit!" do
  it "needs to be reviewed for spec completeness"
end
