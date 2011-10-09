class Struct
  def self.members
    return self::STRUCT_ATTRS.map {|member| member.to_sym }
  end
  
  def members
    return self.class.members
  end
end