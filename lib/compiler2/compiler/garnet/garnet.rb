class RubiniusObject < 'OBJECT'
  def at(idx)
    NTH_FIELD(self, idx)
  end

  def put(idx, obj)
    SET_FIELD(self, idx, obj)
  end
end

declare 'allocate' => 'OBJECT'

def main
  obj = allocate(10)
  obj.put 0, 13.to_object
  obj.at 0
end

