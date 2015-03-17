require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/step', __FILE__)

describe "Numeric#step" do

  describe 'with positional args' do
    it "raises an ArgumentError when step is 0" do
      lambda { 1.step(5, 0) {} }.should raise_error(ArgumentError)
    end

    it "raises an ArgumentError when step is 0.0" do
      lambda { 1.step(2, 0.0) {} }.should raise_error(ArgumentError)
    end

    it_behaves_like :numeric_step, :step

    describe "when no block is given" do
      describe "returned Enumerator" do
        describe "size" do
          it "raises an ArgumentError when step is 0" do
            enum = 1.step(5, 0)
            lambda { enum.size }.should raise_error(ArgumentError)
          end

          it "raises an ArgumentError when step is 0.0" do
            enum = 1.step(2, 0.0)
            lambda { enum.size }.should raise_error(ArgumentError)
          end
        end
      end
    end

  end

  describe 'with keyword arguments' do
    it "doesn't raise an error when step is 0" do
      lambda { 1.step(to: 5, by: 0) { break } }.should_not raise_error
    end

    it "doesn't raise an error when step is 0.0" do
      lambda { 1.step(to: 2, by: 0.0) { break } }.should_not raise_error
    end

    describe "when no block is given" do
      describe "returned Enumerator" do
        describe "size" do
          it "should return Float::INFINITY when step is 0" do
            1.step(to: 5, by: 0).size.should == Float::INFINITY
          end

          it "should return Float::INFINITY when step is 0.0" do
            1.step(to: 2, by: 0.0).size.should == Float::INFINITY
          end
        end
      end
    end

    before :all do
      @args_type = :kw
    end
    it_behaves_like :numeric_step, :step
  end

  describe 'with mixed arguments' do
    it "doesn't raise an error when step is 0" do
      lambda { 1.step(5, by: 0) { break } }.should_not raise_error
    end

    it "doesn't raise an error when step is 0.0" do
      lambda { 1.step(2, by: 0.0) { break } }.should_not raise_error
    end

    it "raises a ArgumentError when limit and to are defined" do
      lambda { 1.step(5, 1, to: 5) { break } }.should raise_error(ArgumentError)
    end

    it "doesn't raise an error when step is 0.0" do
      lambda { 1.step(5, 1, by: 5) { break } }.should raise_error(ArgumentError)
    end

    it "should loop over self when step is 0 or 0.0" do
      1.step(2, by: 0.0).take(5).should eql [1.0, 1.0, 1.0, 1.0, 1.0]
      1.step(2, by: 0).take(5).should eql [1, 1, 1, 1, 1]
      1.1.step(2, by: 0).take(5).should eql [1.1, 1.1, 1.1, 1.1, 1.1]
    end

    describe "when no block is given" do
      describe "returned Enumerator" do
        describe "size" do
          it "should return Float::INFINITY when step is 0" do
            1.step(5, by: 0).size.should == Float::INFINITY
          end

          it "should return Float::INFINITY when step is 0.0" do
            1.step(2, by: 0.0).size.should == Float::INFINITY
          end
        end
      end
    end
    before :all do
      @args_type = :mix
    end
    it_behaves_like :numeric_step, :step
  end
end
