require 'date'

class Class
  def to_yaml( opts = {} )
    raise RbYAML::TypeError, "can't dump anonymous class %s" % self.class
  end
end

class Object
#  yaml_as "tag:ruby.yaml.org,2002:object"
  def is_complex_yaml?; true; end
  def to_yaml_style; end
  def to_yaml_properties; instance_variables.sort; end
  def to_yaml( opts = {} )
    RbYAML::dump(self,nil,opts)
  end
  def taguri
    "!ruby/object:#{self.class.name}"
  end
  def to_yaml_node( repre )
    RbYAML::quick_emit_node( object_id, repre ) do |out|
      mep = {}
      to_yaml_properties.each do |m|
        mep[m[1..-1]] = instance_variable_get(m)
      end      
      out.map(taguri,mep,to_yaml_style)
    end
  end
end

class Hash
  yaml_as "tag:ruby.yaml.org,2002:hash"
  yaml_as "tag:yaml.org,2002:map"
  def is_complex_yaml?; true; end
  def yaml_initialize( tag, val )
    if Array === val
      update Hash.[]( *val )		# Convert the map to a sequence
    elsif Hash === val
      update val
    else
      raise RbYAML::TypeError, "Invalid map explicitly tagged #{ tag }: " + val.inspect
    end
  end
  def to_yaml_node(repre)
    RbYAML::quick_emit_node(object_id, repre) {|rep|
      rep.map(taguri,self,to_yaml_style)
    }
  end
end

class Struct
  yaml_as "tag:ruby.yaml.org,2002:struct"
  def self.yaml_tag_class_name; self.name.gsub( "Struct::", "" ); end
  def self.yaml_tag_read_class( name ); "Struct::#{ name }"; end
  def self.yaml_new( klass, tag, val )
    if Hash === val
      struct_type = nil
      
      #
      # Use existing Struct if it exists
      #
      props = {}
      val.delete_if { |k,v| props[k] = v if k =~ /^@/ }
      begin
        struct_name, struct_type = RbYAML.read_type_class( tag, Struct )
      rescue NameError
      end
      if !struct_type
        struct_def = [ tag.split( /:/, 4 ).last ]
        struct_type = Struct.new( *struct_def.concat( val.keys.collect { |k| k.intern } ) ) 
      end
    
      #
      # Set the Struct properties
      #
      st = RbYAML::object_maker( struct_type, {} )
      st.members.each do |m|
        st.send( "#{m}=", val[m] )
      end
      props.each do |k,v|
        st.instance_variable_set(k, v)
      end
      st
    else
      raise RbYAML::TypeError, "Invalid Ruby Struct: " + val.inspect
    end
  end
  def to_yaml_node( repr)
    RbYAML::quick_emit_node( object_id, repr ) do |out|
      mep = {}
      self.members.each do |m|
        mep[m] = self[m]
      end
      self.to_yaml_properties.each do |m|
        mep[m]=instance_variable_get( m )
      end
      
      #
      # Basic struct is passed as a YAML map
      #
      out.map( taguri, mep, to_yaml_style )
    end
  end
end

class Array
  yaml_as "tag:ruby.yaml.org,2002:array"
  yaml_as "tag:yaml.org,2002:seq"
  def is_complex_yaml?; true; end
  def yaml_initialize( tag, val ); concat( val.to_a ); end
  def to_yaml_node(repre)
    RbYAML::quick_emit_node(object_id, repre) {|rep|
      rep.seq(taguri,self,to_yaml_style)
    }
  end
end

class Exception
  yaml_as "tag:ruby.yaml.org,2002:exception"
  def Exception.yaml_new( klass, tag, val )
    o = RbYAML.object_maker( klass, { 'mesg' => val.delete( 'message' ) } )
    val.each_pair do |k,v|
      o.instance_variable_set("@#{k}", v)
    end
    o
  end
  def to_yaml_node( repr )
    RbYAML::quick_emit_node( object_id, repr ) do |out|
      mep = {}
      mep['message'] = message
      to_yaml_properties.each do |m|
        mep[m[1..-1]] = instance_variable_get( m )
      end
      out.map( taguri, mep, to_yaml_style )
    end
  end
end

class String
  yaml_as "tag:ruby.yaml.org,2002:string"
  yaml_as "tag:yaml.org,2002:binary"
  yaml_as "tag:yaml.org,2002:str"
  def is_complex_yaml?
    to_yaml_style or not to_yaml_properties.empty? or self =~ /\n.+/
  end
  def is_binary_data?
    ( self.count( "^ -~", "^\r\n" ) / self.size > 0.3 || self.count( "\x00" ) > 0 ) unless empty?
  end
  def String.yaml_new( klass, tag, val )
    val = val.unpack("m")[0] if tag == "tag:yaml.org,2002:binary"
    val = { 'str' => val } if String === val
    if Hash === val
      s = klass.allocate
      # Thank you, NaHi
      String.instance_method(:initialize).
        bind(s).
        call( val.delete( 'str' ) )
      val.each { |k,v| s.instance_variable_set( k, v ) }
        s
    else
      raise RbYAML::TypeError, "Invalid String: " + val.inspect
    end
  end
  def to_yaml_node(repre)
    RbYAML::quick_emit_node( is_complex_yaml? ? object_id : nil, repre ) do |rep|
      if is_binary_data?
        rep.scalar( "tag:yaml.org,2002:binary", [self].pack("m"), "|")
      elsif to_yaml_properties.empty?
        rep.scalar(taguri, self, self =~ /^:/ ? '"' : to_yaml_style )
      else
        mep = {}
        mep['str'] = "#{self}"
        to_yaml_properties.each do |m|
          mep.put[m] = instance_variable_get( m )
        end
        rep.map( taguri, mep, to_yaml_style )
      end
    end
  end
end

class Symbol
  yaml_as "tag:ruby.yaml.org,2002:symbol"
  yaml_as "tag:ruby.yaml.org,2002:sym"
  def is_complex_yaml?; false; end
  def Symbol.yaml_new( klass, tag, val )
    if String === val
      val.intern
      else
      raise RbYAML::TypeError, "Invalid Symbol: " + val.inspect
    end
  end
  def to_yaml_node( repr )
    RbYAML::quick_emit_node( nil, repr ) do |out|
      out.scalar( "tag:yaml.org,2002:str", self.inspect, nil )
    end
  end
end

class Range
  yaml_as "tag:ruby.yaml.org,2002:range"
  def Range.yaml_new( klass, tag, val )
    inr = %r'(\w+|[+-]?\d+(?:\.\d+)?(?:e[+-]\d+)?|"(?:[^\\"]|\\.)*")'
    opts = {}
    if String === val and val =~ /^#{inr}(\.{2,3})#{inr}$/o
      r1, rdots, r2 = $1, $2, $3
      opts = {
        'begin' => RbYAML.load( "--- #{r1}" ),
        'end' => RbYAML.load( "--- #{r2}" ),
        'excl' => rdots.length == 3
      }
      val = {}
    elsif Hash === val
      opts['begin'] = val.delete('begin')
      opts['end'] = val.delete('end')
      opts['excl'] = val.delete('excl')
    end
    if Hash === opts
      r = RbYAML::object_maker( klass, {} )
      # Thank you, NaHi
      Range.instance_method(:initialize).
        bind(r).
        call( opts['begin'], opts['end'], opts['excl'] )
      val.each { |k,v| r.instance_variable_set( k, v ) }
      r
    else
      raise RbYAML::TypeError, "Invalid Range: " + val.inspect
    end
  end
  def to_yaml_node( repr )
    RbYAML::quick_emit_node( object_id, repr ) do |out|
      mep = {'begin' => self.begin, 'end' => self.end, 'excl' => self.exclude_end?}
      to_yaml_properties.each do |m|
        mep[m] = instance_variable_get( m )
      end
      out.map( taguri, mep, to_yaml_style )
    end
  end
end

class Regexp
  yaml_as "tag:ruby.yaml.org,2002:regexp"
  def Regexp.yaml_new( klass, tag, val )
    if String === val and val =~ /^\/(.*)\/([mix]*)$/
      val = { 'regexp' => $1, 'mods' => $2 }
    end
    if Hash === val
      mods = nil
      unless val['mods'].to_s.empty?
        mods = 0x00
        mods |= Regexp::EXTENDED if val['mods'].include?( 'x' )
        mods |= Regexp::IGNORECASE if val['mods'].include?( 'i' )
        mods |= Regexp::MULTILINE if val['mods'].include?( 'm' )
      end
      val.delete( 'mods' )
      r = RbYAML::object_maker( klass, {} )
      Regexp.instance_method(:initialize).
        bind(r).
        call( val.delete( 'regexp' ), mods )
      val.each { |k,v| r.instance_variable_set( k, v ) }
      r
    else
      raise RbYAML::TypeError, "Invalid Regular expression: " + val.inspect
    end
  end
  def to_yaml_node( repr )
    RbYAML::quick_emit_node( nil, repr ) do |out|
      if to_yaml_properties.empty?
        out.scalar( taguri, self.inspect, nil )
      else
        mep = { }
        src = self.inspect
        if src =~ /\A\/(.*)\/([a-z]*)\Z/
          mep['regexp']=$1
          mep['mods']=$2
        else
          raise RbYAML::TypeError, "Invalid Regular expression: " + src
        end
        to_yaml_properties.each do |m|
          mep[m] = instance_variable_get( m )
        end
        out.map( taguri, mep, to_yaml_style )
      end
    end
  end
end

class Time
  yaml_as "tag:ruby.yaml.org,2002:time"
  yaml_as "tag:yaml.org,2002:timestamp"
  def is_complex_yaml?; false; end
  def Time.yaml_new( klass, tag, val )
    if Hash === val
      t = val.delete( 'at' )
      val.each { |k,v| t.instance_variable_set( k, v ) }
      t
    else
      raise RbYAML::TypeError, "Invalid Time: " + val.inspect
    end
  end
  def to_yaml_node(repre)
    RbYAML::quick_emit_node( object_id, repre ) do |out|
      tz = "Z"
      # from the tidy Tobias Peters <t-peters@gmx.de> Thanks!
      unless utc?
        utc_same_instant = dup.utc
        utc_same_writing = Time.utc(year,month,day,hour,min,sec,usec)
        difference_to_utc = utc_same_writing - utc_same_instant
        if (difference_to_utc < 0) 
          difference_sign = '-'
          absolute_difference = -difference_to_utc
        else
          difference_sign = '+'
          absolute_difference = difference_to_utc
        end
        difference_minutes = (absolute_difference/60).round
        tz = "%s%02d:%02d" % [ difference_sign, difference_minutes / 60, difference_minutes % 60]
      end
      standard = strftime( "%Y-%m-%d %H:%M:%S" )
      standard += ".%06d" % [usec] if usec.nonzero?
      standard += " %s" % [tz]
      out.scalar(taguri, standard, nil)
    end
  end
end

class Date
  yaml_as "tag:yaml.org,2002:timestamp#ymd"
  def is_complex_yaml?; false; end
  def to_yaml_node(repre)
    RbYAML::quick_emit_node( object_id, repre ) do |out|
      out.scalar( "tag:yaml.org,2002:timestamp", self.to_s, nil )
    end
  end
end

class Numeric
  def is_complex_yaml?; false; end
  def to_yaml_node( repre )
    RbYAML::quick_emit_node( nil, repre ) do |rep|
      str = self.to_s
      if str == "Infinity"
        str = ".inf"
      elsif str == "-Infinity"
        str = "-.inf"
      elsif str == "NaN"
        str = ".nan"
      end
      rep.scalar( taguri, str, nil )
    end
  end
end

class Fixnum
  yaml_as "tag:yaml.org,2002:int"
end

class Float
  yaml_as "tag:yaml.org,2002:float"
end

class TrueClass
  yaml_as "tag:yaml.org,2002:bool#yes"
  def is_complex_yaml?; false; end
  def to_yaml_node( repre )
    RbYAML::quick_emit_node( nil, repre ) do |out|
      out.scalar('tag:yaml.org,2002:bool', "true", nil)
    end
  end
end

class FalseClass
  yaml_as "tag:yaml.org,2002:bool#no"
  def is_complex_yaml?; false; end
  def to_yaml_node( repre )
    RbYAML::quick_emit_node( nil, repre ) do |out|
      out.scalar('tag:yaml.org,2002:bool', "false",nil )
    end
  end
end

class NilClass 
  yaml_as "tag:yaml.org,2002:null"
  def is_complex_yaml?; false; end
  def to_yaml_node( repre )
    RbYAML::quick_emit_node( nil, repre ) do |out|
      out.scalar(taguri, "", nil )
    end
  end
end

