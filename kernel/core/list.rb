# depends on: class.rb

class List
  ivar_as_index :count => 0, :first => 1, :last => 2
  
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

  class Node
    ivar_as_index :object => 0, :next => 1
    
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

