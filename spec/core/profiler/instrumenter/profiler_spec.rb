require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

with_profiler do
  describe "Rubinius::Profiler::Instrumenter" do
    before :each do
      @profiler = Rubinius::Profiler::Instrumenter.new
    end

    it "profiles normal Ruby methods" do
      data = @profiler.profile false do
        ProfilerSpecs.method :work, 3
        ProfilerSpecs.work 10
      end

      work = ProfilerSpecs.find_method(data, "ProfilerSpecs.work")
      work[:called].should == 4
      work[:total].should > 0

      method = ProfilerSpecs.find_method(data, "ProfilerSpecs.method")
      method[:called].should == 1
      method[:total].should > 0
    end

    it "profiles normal Ruby methods that raise exceptions" do
      data = @profiler.profile false do
        begin
          ProfilerSpecs.exceptional 10
        rescue ProfilerSpecs::EarlyOut
        end
      end

      exceptional = ProfilerSpecs.find_method(data, "ProfilerSpecs.exceptional")
      exceptional[:called].should == 1
      exceptional[:total].should > 0
    end

    it "profiles Procs" do
      prc = Proc.new &ProfilerSpecs.load
      data = @profiler.profile false do
        10.times { prc.call }
      end

      fixnum_times = ProfilerSpecs.find_method(data, "Fixnum#*")
      fixnum_times[:called].should == 10
      fixnum_times[:total].should > 0

      load = ProfilerSpecs.find_method(data, "<metaclass>#load {}")
      if load
        load[:called].should == 10
        load[:total].should > 0
      end
    end

    it "profiles blocks" do
      data = @profiler.profile false do
        ProfilerSpecs::Block.new.block
      end

      block_method = ProfilerSpecs.find_method(data, "ProfilerSpecs::Block#block")
      block_method[:called].should == 1
      block_method[:total].should > 0

      block = ProfilerSpecs.find_method(data, "ProfilerSpecs::Block#block {}")
      block.should_not be_nil
      block[:called].should == 5
      block[:total].should > 0
    end

    it "profiles FFI native functions" do
      data = @profiler.profile false do
        ProfilerSpecs.ffi_function 10
      end

      ffi_function = ProfilerSpecs.find_method(data, "ProfilerSpecs.ffi_function")
      ffi_function[:called].should == 1

      rand = ProfilerSpecs.find_method(data, "ProfilerSpecs.rand")
      rand[:called].should == 1000
      rand[:total].should > 0
    end

    it "profiles C-API native methods" do
      require 'digest/md5'
      data = @profiler.profile false do
        digest = Digest::MD5.new
        10.times { |i| digest << i.to_s }
      end

      digest = ProfilerSpecs.find_method(data, "Digest::Base#<<")
      digest[:called].should == 10
      digest[:total].should > 0
    end

    it "profiles primitives" do
      tuple = Rubinius::Tuple[1, 2, 3]
      data = @profiler.profile false do
        10.times { tuple.put 2, 4 }
      end

      tuple[2].should == 4
      put = ProfilerSpecs.find_method(data, "Rubinius::Tuple#put")
      put[:called].should == 10
      put[:total].should > 0
    end
  end
end
