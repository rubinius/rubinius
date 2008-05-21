module ObjectSpaceFixtures
  def self.garbage
    blah
  end

  def self.blah
    o = "hello"
    @garbage_objid = o.object_id
    return o
  end

  @last_objid = nil

  def self.last_objid
    @last_objid
  end

  def self.garbage_objid
    @garbage_objid
  end

  def self.make_finalizer
    proc { |obj_id| @last_objid = obj_id }
  end
end
