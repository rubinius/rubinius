# This benchmark excercises the GC on a large object graph
# It measures how fast the VM is at allocating nodes and reclaiming
# the memory used for old nodes.

require 'benchmark'
require 'benchmark/ips'


# deterministic random
srand(123)

class Splay

  TREE_SIZE = 1000
  TREE_MODIFICATIONS = 80
  PAYLOAD_DEPTH = 5

  def generate_payload_tree(depth, tag)
    if depth == 0
      { :array => [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 ], 
        :string => "String for key #{tag} in leaf node" }
    else
      { :left => generate_payload_tree(depth - 1, tag),
        :right => generate_payload_tree(depth - 1, tag) }
    end
  end

  def generate_key
    rand
  end
  
  def insert_new_node
    key = generate_key
    while @splay_tree.find(key)
      key = generate_key
    end
    payload = generate_payload_tree(PAYLOAD_DEPTH, key.to_s)
    @splay_tree.insert(key, payload)
    key
  end
  
  def splay_setup
    @splay_tree = SplayTree.new
    TREE_SIZE.times { insert_new_node }
  end
  

  def splay_tear_down
    # Allow the garbage collector to reclaim the memory
    # used by the splay tree no matter how we exit the
    # tear down function.
    keys = @splay_tree.export_keys
    @splay_tree = nil
    
    # Verify that the splay tree has the right size.
    if keys.size != TREE_SIZE
      raise "Splay tree has wrong size"
    end
    
    (keys.size-1).times do |idx|
      if keys[idx] >= keys[idx+1]
        raise "Splay tree not sorted"
      end
    end
    
  end

  def splay_run
    # Replace a few nodes in the splay tree.
    TREE_MODIFICATIONS.times do 
      key = insert_new_node
      greatest = @splay_tree.find_greatest_less_than(key)
      if !greatest
        @splay_tree.remove(key) 
      else
        @splay_tree.remove(greatest.key)
      end
    end
    
  end

  class SplayNode
    attr_accessor :key, :value, :left, :right
    def initialize(key, value)
      @key = key
      @value = value
    end

    def traverse(&blk)
      current = self
      while current
        left = current.left
        if left
          left.traverse(&blk)
        end
        yield current
        current = current.right
      end
    end
  end
  
  class SplayTree

    def initialize
      @root = nil
    end
    
    def empty?
      !@root
    end
    

    # Inserts a node into the tree with the specified key and value if
    # the tree does not already contain a node with the specified key. If
    # the value is inserted, it becomes the root of the tree.
    #
    def insert(key, value)
      if empty?
        @root = SplayNode.new(key, value)
        return
      end
      
      # Splay on the key to move the last node on the search path for
      # the key to the root of the tree.
      splay(key)
      if (@root.key == key)
        return
      end
      
      node = SplayNode.new(key, value)
      if key > @root.key 
        node.left = @root
        node.right = @root.right
        @root.right = nil
      else
        node.right = @root
        node.left = @root.left
        @root.left = nil
      end
      @root = node
    end
    

    # Removes a node with the specified key from the tree if the tree
    # contains a node with this key. The removed node is returned. If the
    # key is not found, an exception is thrown.
    #  
    def remove(key)
      if empty?
        raise "Key not found: #{key}"
      end
      
      splay(key)
      if (@root.key != key)
        raise "Key not found: #{key}"
      end
      
      removed = @root
      if !@root.left
        @root = @root.right
      else
        right = @root.right
        @root = @root.left
        # Splay to make sure that the new root has an empty right child.
        splay(key)
        # Insert the original right child as the right child of the new
        # root.
        @root.right = right
      end
      removed
    end


    # Returns the node having the specified key or null if the tree doesn't contain
    # a node with the specified key.
    #  
    # @param {number} key Key to find in the tree.
    # @return {SplayTree.Node} Node having the specified key.
    def find(key)
      if empty?
        return
      end
      
      splay(key)
      @root.key == key ? @root : nil
    end

    def find_max(start_node)
      if empty?
        return
      end
      
      current = start_node || @root
      while current.right
        current = current.right
      end
      current
    end
    

    def find_greatest_less_than(key)
      if empty?
        return
      end
      
      # Splay on the key to move the node with the given key or the last
      # node on the search path to the top of the tree.
      splay(key)
      # Now the result is either the root node or the greatest node in
      # the left subtree.
    
      if @root.key < key
        @root
      elsif @root.left
        find_max(@root.left)
      else
        return
      end
        
    end
    

    def export_keys
      result = []
      unless empty?
        @root.traverse do |node|
          result << node.key
        end
      end
      result
    end
    

    # Perform the splay operation for the given key. Moves the node with
    # the given key to the top of the tree.  If no node has the given
    # key, the last node on the search path is moved to the top of the
    # tree. This is the simplified top-down splaying algorithm from:
    # "Self-adjusting Binary Search Trees" by Sleator and Tarjan


    def splay(key)
      if empty?
        return
      end
      
      # Create a dummy node.  The use of the dummy node is a bit
      # counter-intuitive: The right child of the dummy node will hold
      # the L tree of the algorithm.  The left child of the dummy node
      # will hold the R tree of the algorithm.  Using a dummy node, left
      # and right will always be nodes and we avoid special cases.
      
      dummy = left = right = SplayNode.new(nil, nil)
      current = @root
      while true
        if key < current.key
          if !current.left
            break
          end
          
          if key < current.left.key
            # Rotate right
            tmp = current.left
            current.left = tmp.right
            tmp.right = current
            current = tmp
            if !current.left
              break
            end
          end
          
          # Link right
          right.left = current
          right = current
          current = current.left
        elsif key > current.key
          if !current.right
            break
          end
          if key > current.right.key
            # Rotate left
            tmp = current.right
            current.right = tmp.left
            tmp.left = current
            current = tmp
            if !current.right
              break
            end
          end
          # Link left
          left.right = current
          left = current
          current = current.right
        else
          break
        end
      end
      # end while

      # Assemble
      left.right = current.left
      right.left = current.right
      current.left = dummy.right
      current.right = dummy.left
      @root = current
    end

  end
end

Benchmark.ips do |x|
  x.report 'Splay setup, run and teardown' do
    splay = Splay.new
    splay.splay_setup
    splay.splay_run
    splay.splay_tear_down
  end
end

