@array_slice = shared "Array#[]" do |cmd|
  describe "Array##{cmd}" do
    it "returns the element at index with [index]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 1).should == "b"

      a = [1, 2, 3, 4]
    
      a.send(cmd, 0).should == 1
      a.send(cmd, 1).should == 2
      a.send(cmd, 2).should == 3
      a.send(cmd, 3).should == 4
      a.send(cmd, 4).should == nil
      a.send(cmd, 10).should == nil

      a.should == [1, 2, 3, 4]
    end
  
    it "returns the element at index from the end of the array with [-index]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -2).should == "d"

      a = [1, 2, 3, 4]

      a.send(cmd, -1).should == 4
      a.send(cmd, -2).should == 3
      a.send(cmd, -3).should == 2
      a.send(cmd, -4).should == 1
      a.send(cmd, -5).should == nil
      a.send(cmd, -10).should == nil

      a.should == [1, 2, 3, 4]
    end
  
    it "return count elements starting from index with [index, count]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 2, 3).should == ["c", "d", "e"]

      a = [1, 2, 3, 4]

      a.send(cmd, 0, 0).should == []
      a.send(cmd, 0, 1).should == [1]
      a.send(cmd, 0, 2).should == [1, 2]
      a.send(cmd, 0, 4).should == [1, 2, 3, 4]
      a.send(cmd, 0, 6).should == [1, 2, 3, 4]
      a.send(cmd, 0, -1).should == nil
      a.send(cmd, 0, -2).should == nil
      a.send(cmd, 0, -4).should == nil

      a.send(cmd, 2, 0).should == []
      a.send(cmd, 2, 1).should == [3]
      a.send(cmd, 2, 2).should == [3, 4]
      a.send(cmd, 2, 4).should == [3, 4]
      a.send(cmd, 2, -1).should == nil

      a.send(cmd, 4, 0).should == []
      a.send(cmd, 4, 2).should == []
      a.send(cmd, 4, -1).should == nil

      a.send(cmd, 5, 0).should == nil
      a.send(cmd, 5, 2).should == nil
      a.send(cmd, 5, -1).should == nil

      a.send(cmd, 6, 0).should == nil
      a.send(cmd, 6, 2).should == nil
      a.send(cmd, 6, -1).should == nil

      a.should == [1, 2, 3, 4]
    end
  
    it "returns count elements starting at index from the end of array with [-index, count]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -2, 2).should == ["d", "e"]
    
      a = [1, 2, 3, 4]

      a.send(cmd, -1, 0).should == []
      a.send(cmd, -1, 1).should == [4]
      a.send(cmd, -1, 2).should == [4]
      a.send(cmd, -1, -1).should == nil

      a.send(cmd, -2, 0).should == []
      a.send(cmd, -2, 1).should == [3]
      a.send(cmd, -2, 2).should == [3, 4]
      a.send(cmd, -2, 4).should == [3, 4]
      a.send(cmd, -2, -1).should == nil

      a.send(cmd, -4, 0).should == []
      a.send(cmd, -4, 1).should == [1]
      a.send(cmd, -4, 2).should == [1, 2]
      a.send(cmd, -4, 4).should == [1, 2, 3, 4]
      a.send(cmd, -4, 6).should == [1, 2, 3, 4]
      a.send(cmd, -4, -1).should == nil

      a.send(cmd, -5, 0).should == nil
      a.send(cmd, -5, 1).should == nil
      a.send(cmd, -5, 10).should == nil
      a.send(cmd, -5, -1).should == nil

      a.should == [1, 2, 3, 4]
    end
  
    it "returns the first count elements with [0, count]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 0, 3).should == ["a", "b", "c"]
    end

    it "calls to_int on index and count arguments with [index, count]" do
      obj = Object.new
      def obj.to_int() 2 end
      
      a = [1, 2, 3, 4]
      a.send(cmd, obj).should == 3
      a.send(cmd, obj, 1).should == [3]
      a.send(cmd, obj, obj).should == [3, 4]
      a.send(cmd, 0, obj).should == [1, 2]

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
      obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
      a.send(cmd, obj).should == 3
    end
  
    it "returns the elements specified by Range indexes with [m..n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 1..3).should == ["b", "c", "d"]
      [ "a", "b", "c", "d", "e" ].send(cmd, 4..-1).should == ['e']
      [ "a", "b", "c", "d", "e" ].send(cmd, 3..3).should == ['d']
      [ "a", "b", "c", "d", "e" ].send(cmd, 3..-2).should == ['d']
      ['a'].send(cmd, 0..-1).should == ['a']

      a = [1, 2, 3, 4]
    
      a.send(cmd, 0..-10).should == []
      a.send(cmd, 0..0).should == [1]
      a.send(cmd, 0..1).should == [1, 2]
      a.send(cmd, 0..2).should == [1, 2, 3]
      a.send(cmd, 0..3).should == [1, 2, 3, 4]
      a.send(cmd, 0..4).should == [1, 2, 3, 4]
      a.send(cmd, 0..10).should == [1, 2, 3, 4]
    
      a.send(cmd, 2..-10).should == []
      a.send(cmd, 2..0).should == []
      a.send(cmd, 2..2).should == [3]
      a.send(cmd, 2..3).should == [3, 4]
      a.send(cmd, 2..4).should == [3, 4]

      a.send(cmd, 3..0).should == []
      a.send(cmd, 3..3).should == [4]
      a.send(cmd, 3..4).should == [4]
    
      a.send(cmd, 4..0).should == []
      a.send(cmd, 4..4).should == []
      a.send(cmd, 4..5).should == []

      a.send(cmd, 5..0).should == nil
      a.send(cmd, 5..5).should == nil
      a.send(cmd, 5..6).should == nil

      a.should == [1, 2, 3, 4]
    end
  
    it "returns elements specified by Range indexes except the element at index n with [m...n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 1...3).should == ["b", "c"]

      a = [1, 2, 3, 4]
    
      a.send(cmd, 0...-10).should == []
      a.send(cmd, 0...0).should == []
      a.send(cmd, 0...1).should == [1]
      a.send(cmd, 0...2).should == [1, 2]
      a.send(cmd, 0...3).should == [1, 2, 3]
      a.send(cmd, 0...4).should == [1, 2, 3, 4]
      a.send(cmd, 0...10).should == [1, 2, 3, 4]
    
      a.send(cmd, 2...-10).should == []
      a.send(cmd, 2...0).should == []
      a.send(cmd, 2...2).should == []
      a.send(cmd, 2...3).should == [3]
      a.send(cmd, 2...4).should == [3, 4]
    
      a.send(cmd, 3...0).should == []
      a.send(cmd, 3...3).should == []
      a.send(cmd, 3...4).should == [4]
    
      a.send(cmd, 4...0).should == []
      a.send(cmd, 4...4).should == []
      a.send(cmd, 4...5).should == []
    
      a.send(cmd, 5...0).should == nil
      a.send(cmd, 5...5).should == nil
      a.send(cmd, 5...6).should == nil

      a.should == [1, 2, 3, 4]
    end
  
    it "returns elements that exist if range start is in the array but range end is not with [m..n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 4..7).should == ["e"]
    end
  
    it "accepts Range instances having a negative m and both signs for n with [m..n] and [m...n]" do 
      a = [1, 2, 3, 4]

      a.send(cmd, -1..-1).should == [4]
      a.send(cmd, -1...-1).should == []
      a.send(cmd, -1..3).should == [4]
      a.send(cmd, -1...3).should == []
      a.send(cmd, -1..4).should == [4]
      a.send(cmd, -1...4).should == [4]
      a.send(cmd, -1..10).should == [4]
      a.send(cmd, -1...10).should == [4]
      a.send(cmd, -1..0).should == []
      a.send(cmd, -1..-4).should == []
      a.send(cmd, -1...-4).should == []
      a.send(cmd, -1..-6).should == []
      a.send(cmd, -1...-6).should == []

      a.send(cmd, -2..-2).should == [3]
      a.send(cmd, -2...-2).should == []
      a.send(cmd, -2..-1).should == [3, 4]
      a.send(cmd, -2...-1).should == [3]
      a.send(cmd, -2..10).should == [3, 4]
      a.send(cmd, -2...10).should == [3, 4]

      a.send(cmd, -4..-4).should == [1]
      a.send(cmd, -4..-2).should == [1, 2, 3]
      a.send(cmd, -4...-2).should == [1, 2]
      a.send(cmd, -4..-1).should == [1, 2, 3, 4]
      a.send(cmd, -4...-1).should == [1, 2, 3]
      a.send(cmd, -4..3).should == [1, 2, 3, 4]
      a.send(cmd, -4...3).should == [1, 2, 3]
      a.send(cmd, -4..4).should == [1, 2, 3, 4]
      a.send(cmd, -4...4).should == [1, 2, 3, 4]
      a.send(cmd, -4...4).should == [1, 2, 3, 4]
      a.send(cmd, -4..0).should == [1]
      a.send(cmd, -4...0).should == []
      a.send(cmd, -4..1).should == [1, 2]
      a.send(cmd, -4...1).should == [1]

      a.send(cmd, -5..-5).should == nil
      a.send(cmd, -5...-5).should == nil
      a.send(cmd, -5..-4).should == nil
      a.send(cmd, -5..-1).should == nil
      a.send(cmd, -5..10).should == nil
    
      a.should == [1, 2, 3, 4]
    end

    it "calls to_int on Range arguments with [m..n] and [m...n]" do
      from = Object.new
      to = Object.new

      # So we can construct a range out of them...
      def from.<=>(o) 0 end
      def to.<=>(o) 0 end

      def from.to_int() 1 end
      def to.to_int() -2 end
      
      a = [1, 2, 3, 4]
      
      a.send(cmd, from..to).should == [2, 3]
      a.send(cmd, from...to).should == [2]
      a.send(cmd, 1..0).should == []
      a.send(cmd, 1...0).should == []
    
      should_raise(TypeError) { a.slice("a" .. "b") }
      should_raise(TypeError) { a.slice("a" ... "b") }
      should_raise(TypeError) { a.slice(from .. "b") }
      should_raise(TypeError) { a.slice(from ... "b") }
    
      from = Object.new
      to = Object.new

      def from.<=>(o) 0 end
      def to.<=>(o) 0 end

      from.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
      from.should_receive(:method_missing, :with => [:to_int], :returning => 1)
      to.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
      to.should_receive(:method_missing, :with => [:to_int], :returning => -2)
      a.send(cmd, from..to).should == [2, 3]
    end
  
    it "returns the same elements as [m..n] and [m...n] with Range subclasses" do
      a = [1, 2, 3, 4]
      range_incl = MyRange.new(1, 2)
      range_excl = MyRange.new(-3, -1, true)

      a[range_incl].should == [2, 3]
      a[range_excl].should == [2, 3]
    end

    it "returns nil for a requested index not in the array with [index]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 5).should == nil
    end
  
    it "returns [] if the index is valid but length is zero with [index, length]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 0, 0).should == []
      [ "a", "b", "c", "d", "e" ].send(cmd, 2, 0).should == []
    end

    it "returns nil if length is zero but index is invalid with [index, length]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 100, 0).should == nil
      [ "a", "b", "c", "d", "e" ].send(cmd, -50, 0).should == nil
    end

    # This is by design. It is in the official documentation.
    it "returns [] if index == array.size with [index, length]" do
      %w|a b c d e|.send(cmd, 5, 2).should == []
    end

    it "returns nil if index > array.size with [index, length]" do
      %w|a b c d e|.send(cmd, 6, 2).should == nil
    end

    it "returns nil if length is negative with [index, length]" do
      %w|a b c d e|.send(cmd, 3, -1).should == nil
      %w|a b c d e|.send(cmd, 2, -2).should == nil
      %w|a b c d e|.send(cmd, 1, -100).should == nil
    end
  
    it "returns nil if no requested index is in the array with [m..n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 6..10).should == nil
    end
  
    it "returns nil if range start is not in the array with [m..n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -10..2).should == nil
      [ "a", "b", "c", "d", "e" ].send(cmd, 10..12).should == nil
    end
  
    it "returns an empty array when m == n with [m...n]" do
      [1, 2, 3, 4, 5].send(cmd, 1...1).should == []
    end
  
    it "returns an empty array with [0...0]" do
      [1, 2, 3, 4, 5].send(cmd, 0...0).should == []
    end
  
    it "returns a subarray where m, n negatives and m < n with [m..n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -3..-2).should == ["c", "d"]
    end
  
    it "returns an array containing the first element with [0..0]" do
      [1, 2, 3, 4, 5].send(cmd, 0..0).should == [1]
    end
  
    it "returns the entire array with [0..-1]" do
      [1, 2, 3, 4, 5].send(cmd, 0..-1).should == [1, 2, 3, 4, 5]
    end
  
    it "returns all but the last element with [0...-1]" do
      [1, 2, 3, 4, 5].send(cmd, 0...-1).should == [1, 2, 3, 4]
    end

    it "returns [3] for [2..-1] out of [1, 2, 3] <Specifies bug found by brixen, Defiler, mae>" do
      [1,2,3].send(cmd, 2..-1).should == [3]
    end
  
    it "returns an empty array when m > n and m, n are positive with [m..n]" do
      [1, 2, 3, 4, 5].send(cmd, 3..2).should == []
    end
  
    it "returns an empty array when m > n and m, n are negative with [m..n]" do
      [1, 2, 3, 4, 5].send(cmd, -2..-3).should == []
    end

    it "does not expand array when the indices are outside of the array bounds" do
      a = [1, 2]
      a.send(cmd, 4).should == nil
      a.should == [1, 2]
      a.send(cmd, 4, 0).should == nil
      a.should == [1, 2]
      a.send(cmd, 6, 1).should == nil
      a.should == [1, 2]
      a.send(cmd, 8...8).should == nil
      a.should == [1, 2]
      a.send(cmd, 10..10).should == nil
      a.should == [1, 2]
    end
  
    it "returns a subclass instance when called on a subclass of Array" do
      ary = MyArray[1, 2, 3]
      ary.send(cmd, 0, 0).class.should == MyArray
      ary.send(cmd, 0, 2).class.should == MyArray
      ary.send(cmd, 0..10).class.should == MyArray
    end
  end
end
