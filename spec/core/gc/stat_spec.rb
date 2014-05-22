require File.expand_path('../../../spec_helper', __FILE__)

describe "GC.stat" do
  before :all do
    @stat = GC.stat
  end

  describe "with composite keys" do
    it "has a key for :'gc.young.count'" do
      @stat.key?(:'gc.young.count').should be_true
    end

    it "has a key for :'gc.young.total_wallclock'" do
      @stat.key?(:'gc.young.total_wallclock').should be_true
    end

    it "has a key for :'gc.young.last_wallclock'" do
      @stat.key?(:'gc.young.last_wallclock').should be_true
    end

    it "has a key for :'gc.full.count'" do
      @stat.key?(:'gc.full.count').should be_true
    end

    it "has a key for :'gc.full.total_stop_wallclock'" do
      @stat.key?(:'gc.full.total_stop_wallclock').should be_true
    end

    it "has a key for :'gc.full.total_concurrent_wallclock'" do
      @stat.key?(:'gc.full.total_concurrent_wallclock').should be_true
    end

    it "has a key for :'gc.full.last_stop_wallclock'" do
      @stat.key?(:'gc.full.last_stop_wallclock').should be_true
    end

    it "has a key for :'gc.full.last_concurrent_wallclock'" do
      @stat.key?(:'gc.full.last_concurrent_wallclock').should be_true
    end

    it "has a key for :'memory.counter.young_objects'" do
      @stat.key?(:'memory.counter.young_objects').should be_true
    end

    it "has a key for :'memory.counter.young_bytes'" do
      @stat.key?(:'memory.counter.young_bytes').should be_true
    end

    it "has a key for :'memory.counter.promoted_objects'" do
      @stat.key?(:'memory.counter.promoted_objects').should be_true
    end

    it "has a key for :'memory.counter.promoted_bytes'" do
      @stat.key?(:'memory.counter.promoted_bytes').should be_true
    end

    it "has a key for :'memory.counter.mature_objects'" do
      @stat.key?(:'memory.counter.mature_objects').should be_true
    end

    it "has a key for :'memory.counter.mature_bytes'" do
      @stat.key?(:'memory.counter.mature_bytes').should be_true
    end

    it "has a key for :'memory.young.bytes'" do
      @stat.key?(:'memory.young.bytes').should be_true
    end

    it "has a key for :'memory.mature.bytes'" do
      @stat.key?(:'memory.mature.bytes').should be_true
    end

    it "has a key for :'memory.large.bytes'" do
      @stat.key?(:'memory.large.bytes').should be_true
    end

    it "has a key for :'memory.code.bytes'" do
      @stat.key?(:'memory.code.bytes').should be_true
    end

    it "has a key for :'memory.symbols.bytes'" do
      @stat.key?(:'memory.symbols.bytes').should be_true
    end

    it "has a key for :gc" do
      @stat.key?(:gc).should be_true
    end

    it "has a key for :memory" do
      @stat.key?(:memory).should be_true
    end

    it "does not have a key for :young" do
      @stat.key?(:young).should be_false
    end
  end

  describe "with non-composite keys" do
    it "resolves [:count]" do
      @stat[:count].should be_kind_of(Integer)
    end

    it "resolves [:gc][:young][:count]" do
      @stat[:gc][:young][:count].should be_kind_of(Integer)
    end

    it "resolves [:gc][:young][:total_wallclock]" do
      @stat[:gc][:young][:total_wallclock].should be_kind_of(Integer)
    end

    it "resolves [:gc][:young][:last_wallclock]" do
      @stat[:gc][:young][:last_wallclock].should be_kind_of(Integer)
    end

    it "resolves [:gc][:full][:count]" do
      @stat[:gc][:full][:count].should be_kind_of(Integer)
    end

    it "resolves [:gc][:full][:total_stop_wallclock]" do
      @stat[:gc][:full][:total_stop_wallclock].should be_kind_of(Integer)
    end

    it "resolves [:gc][:full][:total_concurrent_wallclock]" do
      @stat[:gc][:full][:total_concurrent_wallclock].should be_kind_of(Integer)
    end

    it "resolves [:gc][:full][:last_stop_wallclock]" do
      @stat[:gc][:full][:last_stop_wallclock].should be_kind_of(Integer)
    end

    it "resolves [:gc][:full][:last_concurrent_wallclock]" do
      @stat[:gc][:full][:last_concurrent_wallclock].should be_kind_of(Integer)
    end

    it "resolves [:memory][:counter][:young_objects]" do
      @stat[:memory][:counter][:young_objects].should be_kind_of(Integer)
    end

    it "resolves [:memory][:counter][:young_bytes]" do
      @stat[:memory][:counter][:young_bytes].should be_kind_of(Integer)
    end

    it "resolves [:memory][:counter][:promoted_objects]" do
      @stat[:memory][:counter][:promoted_objects].should be_kind_of(Integer)
    end

    it "resolves [:memory][:counter][:promoted_bytes]" do
      @stat[:memory][:counter][:promoted_bytes].should be_kind_of(Integer)
    end

    it "resolves [:memory][:counter][:mature_objects]" do
      @stat[:memory][:counter][:mature_objects].should be_kind_of(Integer)
    end

    it "resolves [:memory][:counter][:mature_bytes]" do
      @stat[:memory][:counter][:mature_bytes].should be_kind_of(Integer)
    end

    it "resolves [:memory][:young][:bytes]" do
      @stat[:memory][:young][:bytes].should be_kind_of(Integer)
    end

    it "resolves [:memory][:mature][:bytes]" do
      @stat[:memory][:mature][:bytes].should be_kind_of(Integer)
    end

    it "resolves [:memory][:large][:bytes]" do
      @stat[:memory][:large][:bytes].should be_kind_of(Integer)
    end

    it "resolves [:memory][:code][:bytes]" do
      @stat[:memory][:code][:bytes].should be_kind_of(Integer)
    end

    it "resolves [:memory][:symbols][:bytes]" do
      @stat[:memory][:symbols][:bytes].should be_kind_of(Integer)
    end
  end
end
