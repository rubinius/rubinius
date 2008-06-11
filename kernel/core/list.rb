# depends on: class.rb

##
# A linked-list data structure.

class List
  ivar_as_index :count => 0, :first => 1, :last => 2

  # List does not have ivars
  def __ivars__ ; nil         ; end
  
  def count
    @count
  end
  
  def first
    @first
  end
  
  def last
    @last
  end
  
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
    ivar_as_index :object => 0, :next => 1

    # Node does not have ivars
    def __ivars__ ; nil         ; end

    
    def object
      @object
    end
    
    def next
      @next
    end

    def initialize(obj)
      @object = obj
      @next = nil
    end

    def next=(obj)
      @next = obj
    end
  end
end

