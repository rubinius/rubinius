module ObjectSpace
  def self.each_object(what=nil, &block)
    unless what == Class
      raise ArgumentError, "ObjectSpace doesn't support '#{what}' yet"
    end
    
    emit_subclasses(Object, block)
  end
  
  def self.emit_subclasses(start, prc)
    subs = start.subclasses
    return 0 unless subs
    count = 0
    
    subs.each do |c|
      count += 1
      prc.call(c)
      count += emit_subclasses(c, prc)
    end
    
    return count
  end
end