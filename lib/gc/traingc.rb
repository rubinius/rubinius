require 'memory'
require 'gc/heap'
require 'gc/gc'

class TrainGC < ForwardingGC
  
  BlockBits = 16
  CarSize = (2 ** BlockBits) - 1
  
  # The number of bits to reshift an address to find the index of the car
  # information from the address.
  IndexMask = 0xff
  
  def initialize(num_trains, num_cars)
    #@address_map = Array.new(IndexMask)
    @address_map = Hash.new
    @trains = []
    @promoted = []
    
    @used_cars = []
    
    num_trains.times do
      train = add_train
      num_cars.times { train.add_car }
    end
    super()
  end
  
  attr_reader :trains, :address_map
  
  def destroy!
    @trains.each do |tr|
      next unless tr
      tr.cars.each { |c| c.deallocate if c }
    end
  end
  
  def allocate(size)
    addr = @newest_car.allocate(size)
    if addr.nil?
      addr = add_train.add_car.allocate(size)
    end
    return addr
  end
  
  def find_lowest_car
    train = @trains.compact.first
    train_idx = train.index
    car_idx = -1
    car = train.cars.find { |o| car_idx += 1; o }
    [train, train_idx, car, car_idx]
  end
  
  def move_object_to_train(obj, train)
    # puts "Moving #{obj.inspect} to #{train.index}"
    if forwarded?(obj)
      nw = forwarding_object(obj)
      # puts "    found forwarded to #{nw.address}"
      update_stack obj, nw
      return nw
    end
    
    nw = train.cars.last.copy_object obj
    if nw.nil?
      nw = train.add_car.copy_object(obj)
    end
    
    # if obj.address == nw.address
    #   puts "Movement failed!"
    # end
    
    # puts "    moved to #{nw.address}"
    
    set_forwarding_address obj, nw
    
    update_stack obj, nw
    nw.references.each do |ref, idx|
      write_barrier nw, ref
    end
    return nw
  end
  
  # Implements the train algorithm collector.
  def invoke(roots)
    processed = []
    intra_train = []
    train, train_idx, car, car_idx = find_lowest_car
    
    # 1. First, check to see whether there are an external references
    # into the train.
    unless train.has_intertrain_references?
      ptr = roots.find do |obj|
        find_address_info(obj).train == train_idx
      end
      # If this is the not the case, then free the entire train.
      if ptr.nil?
        train.destroy!
        @trains[train_idx] = nil
        return
      end
    end
    
    # puts "tracing inter-train.."
    # 2. Otherwise, start cleaning up car as follows:
    #  a. move objects referenced from other trains (as found in
    #     the cars remembered set) to those trains.
    #
    #     process the car's remember_set first.
    # puts "==== #{car.remember_set.inspect}"
    car.remember_set.each do |ext_obj|
      ext_info = find_address_info(ext_obj)
      ext_train = @trains[ext_info.train]
      # puts "== traversing refs of #{ext_obj.inspect}"
      ext_obj.references.each do |ref, ref_idx|
        ref_info = find_address_info(ref)
        # puts "inspecting ref #{ref_idx} #{ref_info.inspect} of #{ext_obj.inspect} #{ext_info.inspect}"
        
        # Only process references into the car being processed.
        if ref_info.train != train_idx or ref_info.car != car_idx
          next
        end
        
        if ext_info.train != ref_info.train
          nw_obj = move_object_to_train ref, ext_train
          ext_obj.put ref_idx, nw_obj
          processed << nw_obj
        elsif ext_info.car != ref_info.car
          intra_train << [ext_obj, ref_idx, ref]
        end
      end      
    end
    
    # puts "tracing roots.."
    #      Next, find if any root objects are in this car.
    roots.each do |root_obj|
      root_info = find_address_info(root_obj)
      if root_info.train == train_idx and 
                        root_info.car == car_idx
        nw_obj = move_object_to_train root_obj, @trains.last
        root_obj.address = nw_obj.address
        processed << root_obj
      end
    end
    
    #  b. Also, move objects being promoted from younger generations
    #     to mature object space into any train, except the from train.
    @promoted.each {}
    @promoted.clear
    
    # 3. Then evacuate the followers in the from car by scanning over the
    #    objects moved in the previous step and evacutaing all reachable
    #    objects into the trains from which they are now referenced.
    # puts "processed: #{processed.inspect}"
    processed.each {}    
    
    # puts "tracing intra-train.."
    # 4. At this point, the from car my still contain objects referenced
    #    from the outside (namely from other cars on the same train.)
    #    Move these objects into the last car of the train, appending
    #    a new car should the train run full.
    intra_train.each do |ext, obj_idx, obj|
      ext_info = find_address_info(ext)
      train = @trains[ext_info.train]
      nw_obj = move_object_to_train obj, train
      ext.put obj_idx, nw_obj
      # puts "Moved #{nw_obj.address}"
      stack = [nw_obj]
      until stack.empty?
        po = stack.pop
        po.references.each do |po_ref, po_ref_idx|
          po_ref_info = find_address_info(po_ref)
          if po_ref_info.train == train_idx and 
                po_ref_info.car == car_idx
            if forwarded?(po_ref)
              po_nw = forwarding_object(po_ref)
            else
              po_nw = move_object_to_train po_ref, train
              stack.push po_nw
            end
            po.put po_ref_idx, po_nw
          end
        end
      end
    end
    
    car.deallocate
    remove_car(train, train_idx, car_idx)
  end
  
  def remove_car(train, train_idx, car_idx)
    train.cars[car_idx] = nil
    @address_map.each do |idx, ary|
      ary.delete_if { |i| i.train == train_idx and i.car == car_idx }
    end
    
    # puts "TG: Removed #{car_idx} from #{train_idx}."
  end
  
  def write_barrier(obj, ref)
    # puts "updating write barrier!"
    info =  find_address_info(obj)
    info2 = find_address_info(ref)
    
    raise "unable to find segment for #{obj}" unless info
    raise "unable to find segment for #{ref}" unless info2
    
    # If it's the exact same address info, then it's the exact same
    # car. In which case, we're done.
    if info.object_id == info2.object_id
      return
    end
    
    # If we've created a reference of an older
    # object into a younger one, we don't need to remember 
    # it.
    if info.train < info2.train
      return
    
    # If we've created a reference of a newer object
    # object in an older one, we need to remember it.
    elsif info.train > info2.train
      # puts "WB: updating inter-train set for #{info2.inspect} (train #{info.train}, added #{obj.address})"
      # puts "WB: obj idx: #{calculate_index(obj.address)} (#{obj.tag}), ref idx: #{calculate_index(ref.address)} (#{ref.tag})"
      # puts "WB: #{info2}"
      car = @trains[info2.train].cars[info2.car]      
      car.remember_set << obj
      car.intertrain_set << obj
    elsif info.car > info2.car
      # puts "WB: updating remember set for #{info2.inspect} (added #{obj.address})"
      @trains[info2.train].cars[info2.car].remember_set << obj
    end
    
  end
  
  def add_train
    train = Train.new(self, @trains.size)
    @trains << train
    return train
  end
  
  def added_car(train, car, car_obj)
    idx1 = calculate_index(car_obj.address)
    idx2 = calculate_index(car_obj.address + car_obj.size)
    
    # puts "WB: (train=#{train}, car=#{car}, #{car_obj.address}) added at #{idx1}, #{idx2}"
    info = AddressInfo.new(train, car, car_obj)
    ary = @address_map[idx1]
    if ary.nil?
      @address_map[idx1] = [info]
    else
      ary << info
    end
    
    if idx1 != idx2
      ary = @address_map[idx2]
      # info = AddressInfo.new(train, car, car_obj)
      if ary.nil?
        # puts "WB: new secondary at #{idx2} for #{info}"
        @address_map[idx2] = [info]
      else
        # puts "WB: overflowed at #{idx2} for #{info}"
        ary << info
      end
    end
    @newest_car = car_obj
  end
  
  def calculate_index(address)
    (address >> BlockBits) & IndexMask
  end
  
  def find_address_info(obj)
    addr = obj.address
    ary = @address_map[calculate_index(addr)]
    return ary.first if ary.size == 1
    ary.each do |info|
      if info.contains?(addr)
        return info
      end
    end
    
    raise "Unable to find info for #{obj}"
  end
  
  class AddressInfo
    def initialize(train, car, car_obj)
      @train = train
      @car = car
      @start = car_obj.address
      @end = car_obj.address + car_obj.size
    end
    
    def contains?(addr)
      addr >= @start and addr < @end
    end
    
    def inspect
      "(train=#{@train}, car=#{@car} #{@start}-#{@end}, #{object_id})"
    end
    
    alias :to_s :inspect
    
    attr_accessor :train, :car
  end
  
  class Train
    attr_reader :index, :cars
    
    def initialize(gc, idx)
      @gc = gc
      @index = idx
      @cars = []
    end
    
    def remember_set
      out = []
      @cars.compact.each do |car|
        out = out + car.intertrain_set
      end
      return out
    end
    
    def add_car
      car = Car.new(CarSize)
      @gc.added_car @index, @cars.size, car
      @cars << car
      return car
    end
    
    def destroy!
      @cars.each { |c| c.deallocate if c }
    end
    
    def has_intertrain_references?
      @cars.compact.each do |car|
        return true unless car.intertrain_set.empty?
      end
      
      return false
    end
    
    def graph(out="", map={})
      out << "digraph structs {\n"
      out << "node [shape=record];\n"
      out << "\n"
    end
  end
  
  class Car < Heap
    def initialize(size)
      super
      @remember_set = []
      @intertrain_set = []
    end
    
    def inspect
      "#<Car #{@address} - #{@address + @size} (#{@size})>"
    end
    
    attr_accessor :remember_set, :intertrain_set
    
    def graph(out="", map={})
      name = "car#{@address}"
      entries = []
      refs = []
      each_object do |obj|
        obj_name = "#{name}:a#{obj.address}"
        entries << "<a#{obj.address}> #{obj.address.to_s(16)}"
        obj.references.each do |ref|
          refs << "#{obj_name} -> a#{ref.address}"
        end
      end
      label = entries.join("|")
      out << "   #{name} [shape=record,label=\"#{label}\"];\n"
      out << refs.join(";\n") + ";\n"
    end
  end
end
