class Class
  def new(*args, &block)
    obj = allocate()
    obj.initialize(*args, &block)
    obj
  end

  def self.new(sclass=Object, &block)
    obj = allocate()
    obj.superclass = sclass
    obj.class_eval(&block) if block
    obj
  end
end
