@hash_store = shared "Hash#store" do |cmd|
  describe "Hash##{cmd}" do
    it "associates the key with the value and return the value" do
      h = { :a => 1 }
      h.send(cmd, :b, 2).should == 2
      h.should == {:b=>2, :a=>1}
    end

    it "duplicates and freezes string keys" do
      key = "foo"
      h = {}
      h.send(cmd, key, 0)
      key << "bar"

      h.should == { "foo" => 0 }
      h.keys[0].frozen?.should == true
    end

    it "duplicates string keys using dup semantics" do
      # dup doesn't copy singleton methods
      key = "foo"
      def key.reverse() "bar" end
      h = {}
      h.send(cmd, key, 0)

      h.keys[0].reverse.should == "oof"
    end  

    compliant :mri do
      it "raises TypeError if called on a frozen instance" do
        should_raise(TypeError) { @hash.send(cmd, 1, 2) }
      end
    end
  end
end
