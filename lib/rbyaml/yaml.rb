require 'stringio'

require 'rbyaml/error'

require 'rbyaml/tokens'
require 'rbyaml/events'
require 'rbyaml/nodes'

require 'rbyaml/loader'
require 'rbyaml/dumper'

module RbYAML
  def self._scan(stream, loader=Loader)
    l = loader.new(stream)
    yield l.scanner.get_token while l.scanner.check_token
  end

  def self._parse(stream, loader=Loader)
    l = loader.new(stream)
    yield l.parser.get_event while l.parser.check_event
  end

  def self._compose(stream, loader=Loader)
    l = loader.new(stream)
    l.composer.get_node if l.composer.check_node
  end

  def self._compose_all(stream, loader=Loader)
    l = loader.new(stream)
    yield l.composer.get_node while l.composer.check_node
  end

  def self._load_all(stream, loader=Loader)
    l = loader.new(stream)
    yield l.constructor.get_data while l.constructor.check_data
  end
  
  def self._load(stream, loader=Loader)
    l = loader.new(stream)
    l.constructor.get_data if l.constructor.check_data
  end

  def self._safe_load_all(stream)
    _load_all(stream, SafeLoader)
  end

  def self._safe_load(stream)
    _load(stream, SafeLoader)
  end

  def self._emit(events, stream=nil, dump=Dumper, opts={})
    if stream.nil?
      require 'stringio'
      stream = StringIO.new
    end
    dumper = dump.new(stream,opts)
    for event in events
      dumper.emit(event)
    end        
    stream.string if StringIO === stream
  end

  def self._serialize_all(nodes,stream=nil, dump=Dumper, opts={})
    if stream.nil?
      require 'stringio'
      stream = StringIO.new
    end
    dumper = dump.new(stream,opts)
    dumper.serializer.open
    for node in nodes
      dumper.serializer.serialize(node)
    end
    dumper.serializer.close
    stream.string if StringIO === stream
  end
  
  def self._serialize(node, stream=nil, dump=Dumper, opts={})
    _serialize_all([node], stream, dump, opts)
  end
  
  def self._dump_all(documents,stream=nil, dump=Dumper, opts={})
    if stream.nil?
      require 'stringio'
      stream = StringIO.new
    end
    dumper = dump.new(stream,opts)
    dumper.serializer.open
    for data in documents
      dumper.representer.represent(data)
    end
    dumper.serializer.close
    stream.string if StringIO === stream
  end

  def self._dump(data, stream=nil,dump=Dumper, opts={})
    _dump_all([data], stream, dump, opts)
  end
  
  def self._add_implicit_resolver(tag, regexp, first=nil, resolver=BaseResolver)
    resolver.add_implicit_resolver(tag, regexp, first)
  end

  def self._add_path_resolver(tag, path, kind=nil, resolver=BaseResolver)
    resolver.add_path_resolver(tag, path, kind)
  end
  
  def self._add_constructor(tag, constructor, ctor=Constructor)
    ctor.add_constructor(tag, constructor)
  end

  def self._add_multi_constructor(tag_prefix, multi_constructor, ctor=Constructor)
    ctor.add_multi_constructor(tag_prefix, multi_constructor)
  end

  def self._add_representer(data_type, representer, rep=BaseRepresenter)
    rep.add_representer(data_type, representer)
  end

  def self._add_multi_representer(data_type, multi_representer, rep=BaseRepresenter)
    rep.add_multi_representer(data_type, multi_representer)
  end

  def self._dump_ruby_object(data, dumper=Dumper)
    _dump(data,nil,dumper)
  end
end
