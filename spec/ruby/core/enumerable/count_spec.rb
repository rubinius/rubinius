require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.8.7" do
  describe "Enumerable#count" do
    before :each do
      @elements = [1, 2, 4, 2]
      @numerous = EnumerableSpecs::Numerous.new(*@elements)
    end

    describe "when no argument or a block" do
      it "returns size" do
        @numerous.count.should == 4
      end

      describe "with a custom size method" do
        before(:each) do
          class << @numerous
            def size
              :any_object
            end
          end
        end

        ruby_version_is ""..."1.9" do
          it "uses the custom size method" do
            @numerous.count.should == :any_object
          end
        end

        ruby_version_is "1.9" do
          it "ignores the custom size method" do
            @numerous.count.should == 4
          end
        end
      end
    end

    it "counts nils if given nil as an argument" do
      EnumerableSpecs::Numerous.new(nil, nil, nil, false).count(nil).should == 3
    end

    it "accepts an argument for comparison using ==" do
      @numerous.count(2).should == 2
    end

    it "uses a block for comparison" do
      @numerous.count{|x| x%2==0 }.should == 3
    end

    it "ignores the block when given an argument" do
      @numerous.count(4){|x| x%2==0 }.should == 1
    end

    ruby_version_is ""..."1.9" do
      it "gathers whole arrays as elements when each yields multiple" do
        multi = EnumerableSpecs::YieldsMulti.new
        multi.count {|e| e == [1, 2]}.should == 1
      end
    end

    ruby_version_is "1.9" do
      it "gathers initial args as elements when each yields multiple" do
        multi = EnumerableSpecs::YieldsMulti.new
        multi.count {|e| e == 1 }.should == 1
      end
    end
  end
end
