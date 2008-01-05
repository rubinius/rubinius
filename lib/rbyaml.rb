# Copyright (c) 2006 Ola Bini
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

require 'rbyaml/yaml'
require 'rbyaml/stream'
require 'rbyaml/dumper'

module RbYAML
  def self.dump(obj, io = nil, opts={})
    _dump(obj,io,Dumper,opts)
  end

  def self.load( io )
    _load(io)
  end

  def self.load_file( filepath )
    File.open( filepath ) do |f|
      load( f )
    end
  end

  # this operation does not make sense in RbYAML (right now)
  def self.parse( io )
    raise NotImplementedError
    #    yp = @@parser.new( :Model => :Generic ).load( io )
  end

  # this operation does not make sense in RbYAML (right now)
  def self.parse_file( filepath )
    raise NotImplementedError
    #    File.open( filepath ) do |f|
    #      parse( f )
    #    end
  end

  def self.each_document( io, &block )
    _load_all(io,&block)
  end

  def self.load_documents( io, &doc_proc )
    each_document( io, &doc_proc )
  end

  # this operation does not make sense in RbYAML (right now)
  def self.each_node( io, &doc_proc )
    raise NotImplementedError
    #    yp = @@parser.new( :Model => :Generic ).load_documents( io, &doc_proc )
  end

  # this operation does not make sense in RbYAML (right now)
  def self.parse_documents( io, &doc_proc )
    raise NotImplementedError
    #    YAML.each_node( io, &doc_proc )
  end
  
  def self.load_stream( io )
    d = nil
    load_documents(io) { |doc|
      d = Stream.new( nil ) if not d
      d.add( doc ) 
    }
    d
  end

  def self.dump_stream( *objs )
    d = RbYAML::Stream.new
    objs.each do |doc|
      d.add( doc ) 
    end
    d.emit
  end


  def self.add_builtin_ctor(type_tag, &transfer_proc)
    BaseConstructor::add_constructor("tag:yaml.org,2002:#{ type_tag }",transfer_proc)
  end

  # this operation does not make sense in RbYAML (right now)
  def self.add_domain_type( domain, type_re, &transfer_proc )
    raise NotImplementedError
    #    @@loader.add_domain_type( domain, type_re, &transfer_proc )
  end

  # this operation does not make sense in RbYAML (right now)
  def self.add_builtin_type( type_re, &transfer_proc )
    raise NotImplementedError
    #    @@loader.add_builtin_type( type_re, &transfer_proc )
  end

  # this operation does not make sense in RbYAML (right now)
  def self.add_ruby_type( type_tag, &transfer_proc )
    raise NotImplementedError
    #    @@loader.add_ruby_type( type, &transfer_proc )
  end

  # this operation does not make sense in RbYAML (right now)
  def self.add_private_type( type_re, &transfer_proc )
    raise NotImplementedError
    #    @@loader.add_private_type( type_re, &transfer_proc )
  end

  def self.detect_implicit( val )
    SimpleDetector.detect(val)
  end

  # this operation does not make sense in RbYAML (right now)
  def self.transfer( type_id, obj )
    raise NotImplementedError
    #    @@loader.transfer( type_id, obj )
  end

  # this operation does not make sense in RbYAML (right now)
  def self.try_implicit( obj )
    raise NotImplementedError
    #    YAML.transfer( YAML.detect_implicit( obj ), obj )
  end

  def self.read_type_class( type, obj_class )
    scheme, domain, type, tclass = type.split( ':', 4 )
    tclass.split( "::" ).each { |c| obj_class = obj_class.const_get( c ) } if tclass
    return [ type, obj_class ]
  end

  def self.object_maker( obj_class, val )
    if Hash === val
      o = obj_class.allocate
      val.each_pair { |k,v|
        o.instance_variable_set("@#{k}", v)
      }
      o
    else
      raise YAMLError, "Invalid object explicitly tagged !ruby/Object: " + val.inspect
    end
  end

  def self.quick_emit(oid, opts = {})
    if Dumper === opts then
      rep = opts
    else
      stream = StringIO.new
      dumper = Dumper.new stream, opts
      dumper.serializer.open
      rep = dumper.representer
    end

    node = RbYAML.quick_emit_node oid, rep do |out|
      yield out

      #map = {}
      #to_yaml_properties.each do |m|
      #  map[m[1..-1]] = instance_variable_get(m)
      #end
      #out.map taguri, map, to_yaml_style
    end

    dumper.serializer.serialize node

    dumper.serializer.close
    stream.string
  end

  def self.quick_emit_node( oid, rep, &e )
    e.call(rep)
  end
end

require 'rbyaml/tag'
require 'rbyaml/types'
require 'rbyaml/rubytypes'
