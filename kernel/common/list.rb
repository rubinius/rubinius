# depends on: class.rb

##
# A linked-list data structure.

class List
  attr_reader :count
  attr_reader :first
  attr_reader :last
  
  def initialize
    @count = 0
    @first = nil
    @last = nil
  end

  def <<(obj)
    node = Node.new(obj)
    unless @first
      @first = node
    end

    if @last
      @last.next = node
      @last = node
    else
      @last = node
    end

    @count += 1
  end

  def shift
    return nil unless @count > 0
    node = @first
    @first = node.next
    @count -= 1
    return node.object
  end

  def each(&blk)
    node = @first
    while node
      blk.call node.object
      node = node.next
    end

    return nil
  end

  def inspect
    "#<List:0x%x @count=%p>" % [object_id, count]
  end

  class Node
    attr_reader :object
    attr_accessor :next

    def initialize(obj)
      @object = obj
      @next = nil
    end
  end
end

