@hash_iteration_method = shared "Iteration method" do |cmd|
  # These are the only ones that actually have the exceptions on MRI 1.8.
  # sort and reject don't raise!
  # 
  #   delete_if each each_key each_pair each_value merge merge! reject!
  #   select update
  #
  describe "Hash##{cmd}" do
    hsh = {1 => 2, 3 => 4, 5 => 6}  
    big_hash = {}
    100.times { |k| big_hash[k.to_s] = k }    
       
    it "raise RuntimeError if #rehash is called from block" do
      h = hsh.dup
      args = cmd.to_s[/merge|update/] ? [h] : []
      
      should_raise(RuntimeError, "rehash occurred during iteration") do
        h.send(cmd, *args) { h.rehash }
      end
    end

    it "raises if lots of new entries are added from block" do
      h = hsh.dup
      args = cmd.to_s[/merge|update/] ? [h] : []

      should_raise(RuntimeError, "hash modified during iteration") do
        h.send(cmd, *args) { |*x| h.merge!(big_hash) }
      end
    end

  end
end

@hash_iteration_modifying = shared "Iteration modifying" do |cmd|
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
