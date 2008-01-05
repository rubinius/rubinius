shared :hash_iteration_method do |cmd|
  # These are the only ones that actually have the exceptions on MRI 1.8.
  # sort and reject don't raise!
  # 
  #   delete_if each each_key each_pair each_value merge merge! reject!
  #   select update
  #
  describe "Hash##{cmd}" do
    hsh = {1 => 2, 3 => 4, 5 => 6}  
    big_hash = {}
    64.times { |k| big_hash[k.to_s] = k }    
       
    # it "raises a RuntimeError if #rehash is called from block" do
    #   h = hsh.dup
    #   args = cmd.to_s[/merge|update/] ? [h] : []
    #   
    #   lambda {
    #     h.send(cmd, *args) { h.rehash }
    #   }.should raise_error(RuntimeError)
    # end
    # 
    # # This specification seems arbitrary, but describes the behavior of MRI
    # it "raises if more than 63 new entries are added from block" do
    #   h = hsh.dup
    #   args = cmd.to_s[/merge|update/] ? [h] : []
    # 
    #   lambda {
    #     h.send(cmd, *args) { |*x| h.merge!(big_hash) }
    #   }.should raise_error(RuntimeError)
    # end

  end
end

shared :hash_iteration_modifying do |cmd|
  describe "Hash##{cmd}" do
    hsh = {1 => 2, 3 => 4, 5 => 6}  
    big_hash = {}
    100.times { |k| big_hash[k.to_s] = k }    
       
    it "does not affect yielded items by removing the current element" do
      n = 3
    
      h = Array.new(n) { hsh.dup }
      args = Array.new(n) { |i| cmd.to_s[/merge|update/] ? [h[i]] : [] }
      r = Array.new(n) { [] }
    
      h[0].send(cmd, *args[0]) { |*x| r[0] << x; true }
      h[1].send(cmd, *args[1]) { |*x| r[1] << x; h[1].shift; true }
      h[2].send(cmd, *args[2]) { |*x| r[2] << x; h[2].delete(h[2].keys.first); true }
    
      r[1..-1].each do |yielded|
        yielded.should == r[0]
      end
    end
  end
end

shared :hash_iteration_no_block do |cmd|
  describe "Hash##{cmd}" do
    hsh = {1 => 2, 3 => 4, 5 => 6}  
    empty = {}
    
    it "raises a LocalJumpError when called on a non-empty hash without a block" do
      lambda { hsh.delete_if }.should raise_error(LocalJumpError)
    end
    
    it "does not raise a LocalJumpError when called on an empty hash without a block" do
      empty.delete_if.should == empty
    end
  end
end