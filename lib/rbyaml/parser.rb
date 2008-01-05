
# YAML can be parsed by an LL(1) parser!
#
# We use the following production rules:
# stream            ::= STREAM-START implicit_document? explicit_document* STREAM-END
# explicit_document ::= DIRECTIVE* DOCUMENT-START block_node? DOCUMENT-END?
# implicit_document ::= block_node DOCUMENT-END?
# block_node    ::= ALIAS | properties? block_content
# flow_node     ::= ALIAS | properties? flow_content
# properties    ::= TAG ANCHOR? | ANCHOR TAG?
# block_content     ::= block_collection | flow_collection | SCALAR
# flow_content      ::= flow_collection | SCALAR
# block_collection  ::= block_sequence | block_mapping
# block_sequence    ::= BLOCK-SEQUENCE-START (BLOCK-ENTRY block_node?)* BLOCK-END
# block_mapping     ::= BLOCK-MAPPING_START ((KEY block_node_or_indentless_sequence?)? (VALUE block_node_or_indentless_sequence?)?)* BLOCK-END
# block_node_or_indentless_sequence ::= ALIAS | properties? (block_content | indentless_block_sequence)
# indentless_block_sequence         ::= (BLOCK-ENTRY block_node?)+
# flow_collection   ::= flow_sequence | flow_mapping
# flow_sequence     ::= FLOW-SEQUENCE-START (flow_sequence_entry FLOW-ENTRY)* flow_sequence_entry? FLOW-SEQUENCE-END
# flow_mapping      ::= FLOW-MAPPING-START (flow_mapping_entry FLOW-ENTRY)* flow_mapping_entry? FLOW-MAPPING-END
# flow_sequence_entry   ::= flow_node | KEY flow_node? (VALUE flow_node?)?
# flow_mapping_entry    ::= flow_node | KEY flow_node? (VALUE flow_node?)?

# TODO: support for BOM within a stream.
# stream ::= (BOM? implicit_document)? (BOM? explicit_document)* STREAM-END

# FIRST sets:
# stream: { STREAM-START }
# explicit_document: { DIRECTIVE DOCUMENT-START }
# implicit_document: FIRST(block_node)
# block_node: { ALIAS TAG ANCHOR SCALAR BLOCK-SEQUENCE-START BLOCK-MAPPING-START FLOW-SEQUENCE-START FLOW-MAPPING-START }
# flow_node: { ALIAS ANCHOR TAG SCALAR FLOW-SEQUENCE-START FLOW-MAPPING-START }
# block_content: { BLOCK-SEQUENCE-START BLOCK-MAPPING-START FLOW-SEQUENCE-START FLOW-MAPPING-START SCALAR }
# flow_content: { FLOW-SEQUENCE-START FLOW-MAPPING-START SCALAR }
# block_collection: { BLOCK-SEQUENCE-START BLOCK-MAPPING-START }
# flow_collection: { FLOW-SEQUENCE-START FLOW-MAPPING-START }
# block_sequence: { BLOCK-SEQUENCE-START }
# block_mapping: { BLOCK-MAPPING-START }
# block_node_or_indentless_sequence: { ALIAS ANCHOR TAG SCALAR BLOCK-SEQUENCE-START BLOCK-MAPPING-START FLOW-SEQUENCE-START FLOW-MAPPING-START BLOCK-ENTRY }
# indentless_sequence: { ENTRY }
# flow_collection: { FLOW-SEQUENCE-START FLOW-MAPPING-START }
# flow_sequence: { FLOW-SEQUENCE-START }
# flow_mapping: { FLOW-MAPPING-START }
# flow_sequence_entry: { ALIAS ANCHOR TAG SCALAR FLOW-SEQUENCE-START FLOW-MAPPING-START KEY }
# flow_mapping_entry: { ALIAS ANCHOR TAG SCALAR FLOW-SEQUENCE-START FLOW-MAPPING-START KEY }

require 'rbyaml/error'
require 'rbyaml/tokens'
require 'rbyaml/events'
require 'rbyaml/scanner'

module RbYAML
  class ParserError < YAMLError
  end

  class Parser
    DEFAULT_TAGS = {
      '!' => '!',
      '!!' => 'tag:yaml.org,2002:'
    }

    DOCUMENT_END_TRUE = DocumentEndEvent.new(true)
    DOCUMENT_END_FALSE = DocumentEndEvent.new(false)
    MAPPING_END = MappingEndEvent.new
    SEQUENCE_END = SequenceEndEvent.new
    STREAM_END = StreamEndEvent.new
    STREAM_START = StreamStartEvent.new
    

    def initialize(scanner)
      @scanner = scanner
      @current_event = nil
      @yaml_version = nil
      @events = nil
      @working_events = nil
      @tag_handles = { }
      @parse_stack = nil
    end

    def check_event(*choices)
      parse_stream
      @current_event = parse_stream_next if @current_event.nil?
      if @current_event
        return true if choices.empty?
        for choice in choices
          return true if choice === @current_event
        end
      end
      false
    end

    def peek_event
      parse_stream
      @current_event = parse_stream_next unless @current_event
      @current_event
    end

    def get_event
      parse_stream
      @current_event = parse_stream_next unless @current_event
      value = @current_event
      @current_event = nil
      value
    end

    def each_event
      parse_stream
      while @current_event = parse_stream_next
        yield @current_event
      end
    end

    def parse_stream
      if !@parse_stack
        @parse_stack = [:stream]
        @tags = []
        @anchors = []
      end
    end

    def parse_stream_next
      if !@parse_stack.empty?
        while true
          meth = @parse_stack.pop
#puts "our method: :#{meth}"
#puts "--- with peeked: :#{@scanner.peek_token.class} #{if @scanner.peek_token.respond_to?(:value): @scanner.peek_token.value.inspect; end}"
          val = send(meth)
          if !val.nil?
#puts "returning: #{val}"
            return val
          end
        end
      else
        @tags = []
        @anchors = []
        return nil
      end
    end

#TERMINALS, definitions

    def stream_start
      @scanner.get_token
      STREAM_START
    end

    def stream_end
      @scanner.get_token
      STREAM_END
    end
    
    def document_start_implicit
      token = @scanner.peek_token
      version, tags = process_directives
      DocumentStartEvent.new(false,version,tags)
    end

    def document_start
      token = @scanner.peek_token
      version, tags = process_directives
      raise ParserError.new(nil, "expected '<document start>', but found #{token.tid}") unless @scanner.peek_token.__is_document_start
      @scanner.get_token
      DocumentStartEvent.new(true,version,tags)
    end

    def document_end
      token = @scanner.peek_token
      explicit = false
      while @scanner.peek_token.__is_document_end
        @scanner.get_token
        explicit = true
      end
      explicit ? DOCUMENT_END_TRUE : DOCUMENT_END_FALSE
    end

    def _alias
      AliasEvent.new(@scanner.get_token.value)
    end

    def block_sequence_start
      implicit = @tags.last.nil? || @tags.last == "!"
      @scanner.get_token
      SequenceStartEvent.new(@anchors.last, @tags.last, implicit, false)
    end

    def block_indentless_sequence_start
      implicit = @tags.last.nil? || @tags.last == "!"
      SequenceStartEvent.new(@anchors.last, @tags.last, implicit, false)
    end

    def block_sequence_end
      if !@scanner.peek_token.__is_block_end
        raise ParserError.new("while scanning a block collection","expected <block end>, but found #{token.tid}: #{token.inspect}")
      end
      @scanner.get_token
      SEQUENCE_END
    end

    def block_indentless_sequence_end
      SEQUENCE_END
    end

    def block_mapping_start
      implicit = @tags.last.nil? || @tags.last == "!"
      @scanner.get_token
      MappingStartEvent.new(@anchors.last, @tags.last, implicit,false)
    end

    def block_mapping_end
      if !@scanner.peek_token.__is_block_end
        raise ParserError.new("while scanning a block mapping","expected <block end>, but found #{token.tid}")
      end
      @scanner.get_token
      MAPPING_END
    end

    def flow_sequence_start
      implicit = @tags.last.nil? || @tags.last == "!"
      @scanner.get_token
      SequenceStartEvent.new(@anchors.last, @tags.last, implicit,true)
    end

    def flow_sequence_end
      @scanner.get_token
      SEQUENCE_END
    end

    def flow_internal_mapping_start
      @scanner.get_token
      MappingStartEvent.new(nil,nil,true,true)
    end

    def flow_internal_mapping_end
      MAPPING_END
    end

    def flow_mapping_start
      implicit = @tags.last.nil? || @tags.last == "!"
      @scanner.get_token
      MappingStartEvent.new(@anchors.last, @tags.last, implicit,true)
    end

    def flow_mapping_end
      @scanner.get_token
      MAPPING_END
    end

    def scalar
      token = @scanner.get_token
      if (token.plain && @tags.last.nil?) || @tags.last == "!"
        implicit = [true, false]
      elsif @tags.last.nil?
        implicit = [false, true]
      else
        implicit = [false, false]
      end
      ScalarEvent.new(@anchors.last, @tags.last, implicit, token.value,token.style)
    end

    def empty_scalar
      process_empty_scalar
    end
    

# PRODUCTIONS
    def stream
      @parse_stack += [:stream_end, :explicit_document, :implicit_document]
      stream_start
    end

    def implicit_document
      curr = @scanner.peek_token
      unless curr.__is_directive || curr.__is_document_start || curr.__is_stream_end
        @parse_stack += [:document_end, :block_node]
        return document_start_implicit
      end
      nil
    end

    def explicit_document
      if !@scanner.peek_token.__is_stream_end
        @parse_stack += [:explicit_document, :document_end, :block_node]
        return document_start
      end
      nil
    end

    def block_node
      curr = @scanner.peek_token
      if curr.__is_directive || curr.__is_document_start || curr.__is_document_end || curr.__is_stream_end
        return empty_scalar
      else
        if curr.__is_alias
          return _alias
        else
          @parse_stack << :un_properties
          properties
          return block_content
        end
      end
    end

    def flow_node
      if @scanner.peek_token.__is_alias
        return _alias
      else
        @parse_stack << :un_properties
        properties
        return flow_content
      end
    end

    def properties
      anchor = nil
      tag = nil
      if @scanner.peek_token.__is_anchor
        anchor = @scanner.get_token.value
        if @scanner.peek_token.__is_tag
          tag = @scanner.get_token.value
        end
      elsif @scanner.peek_token.__is_tag
        tag = @scanner.get_token.value
        if @scanner.peek_token.__is_anchor
          anchor = @scanner.get_token.value
        end
      end
      
      if !tag.nil? and tag != "!"
        handle, suffix = tag
        if !handle.nil?
          raise ParserError.new("while parsing a node","found undefined tag handle #{handle}") if !@tag_handles.include?(handle)
          tag = @tag_handles[handle]+suffix
        else
          tag = suffix
        end
      end
      @anchors << anchor
      @tags << tag
      nil
    end

    def un_properties
      @anchors.pop
      @tags.pop
      nil
    end

    def block_content
      token = @scanner.peek_token
      if token.__is_block_sequence_start
        return block_sequence
      elsif token.__is_block_mapping_start
        return block_mapping
      elsif token.__is_flow_sequence_start
        return flow_sequence
      elsif token.__is_flow_mapping_start
        return flow_mapping
      elsif token.__is_scalar
        return scalar
      else
        raise ParserError.new("while scanning a node","expected the node content, but found #{token.tid}")
      end
    end

    def flow_content
      token = @scanner.peek_token
      if token.__is_flow_sequence_start
        return flow_sequence
      elsif token.__is_flow_mapping_start
        return flow_mapping
      elsif token.__is_scalar
        return scalar
      else
        raise ParserError.new("while scanning a flow node","expected the node content, but found #{token.tid}")
      end
    end

    def block_sequence_entry
      if @scanner.peek_token.__is_block_entry
        @scanner.get_token
        if !(@scanner.peek_token.__is_block_entry || @scanner.peek_token.__is_block_end)
          @parse_stack += [:block_sequence_entry]
          return block_node
        else
          @parse_steck += [:block_sequence_entry]
          return empty_scalar
        end
      end      
      nil
    end

    def block_mapping_entry
      #   ((KEY block_node_or_indentless_sequence?)? (VALUE block_node_or_indentless_sequence?)?)*
      if @scanner.peek_token.__is_key || @scanner.peek_token.__is_value
        if @scanner.check_token(KeyToken)
          @scanner.get_token
          curr = @scanner.peek_token
          if !(curr.__is_key || curr.__is_value || curr.__is_block_end)
            @parse_stack += [:block_mapping_entry,:block_mapping_entry_value]
            return block_node_or_indentless_sequence
          else
            @parse_stack += [:block_mapping_entry,:block_mapping_entry_value]
            return empty_scalar
          end
        else
          @parse_stack += [:block_mapping_entry,:block_mapping_entry_value]
          return empty_scalar
        end
      end
      nil
    end

    def block_mapping_entry_value
      if @scanner.peek_token.__is_key || @scanner.peek_token.__is_value
        if @scanner.peek_token.__is_value
          @scanner.get_token
          curr = @scanner.peek_token
          if !(curr.__is_key || curr.__is_value || curr.__is_block_end)
            return block_node_or_indentless_sequence
          else
            return empty_scalar
          end
        else
          return empty_scalar
        end
      end
      nil
    end

    def block_sequence
      @parse_stack += [:block_sequence_end,:block_sequence_entry]
      block_sequence_start
    end

    def block_mapping
      @parse_stack += [:block_mapping_end,:block_mapping_entry]
      block_mapping_start
    end

    def block_node_or_indentless_sequence
      if @scanner.peek_token.__is_alias
        return _alias
      else
        if @scanner.peek_token.__is_block_entry
          properties
          return indentless_block_sequence
        else
          properties
          return block_content
        end
      end
    end

    def indentless_block_sequence
      @parse_stack += [:block_indentless_sequence_end,:indentless_block_sequence_entry]
      block_indentless_sequence_start
    end

    def indentless_block_sequence_entry
      if @scanner.peek_token.__is_block_entry
        @scanner.get_token
        curr = @scanner.peek_token
        if !(curr.__is_block_entry || curr.__is_key || curr.__is_value || curr.__is_block_end)
          @parse_stack << :indentless_block_sequence_entry
          return block_node
        else
          @parse_stack << :indentless_block_sequence_entry
          return empty_scalar
        end
      end
      nil
    end

    def flow_sequence
      @parse_stack += [:flow_sequence_end,:flow_sequence_entry]
      flow_sequence_start
    end

    def flow_mapping
      @parse_stack += [:flow_mapping_end,:flow_mapping_entry]
      flow_mapping_start
    end

    def flow_sequence_entry
      if !@scanner.peek_token.__is_flow_sequence_end
        if @scanner.peek_token.__is_key
          @parse_stack += [:flow_sequence_entry,:flow_entry_marker,:flow_internal_mapping_end,:flow_internal_value,:flow_internal_content]
          return flow_internal_mapping_start
        else
          @parse_stack += [:flow_sequence_entry,:flow_node]
          return flow_entry_marker
        end
      end
      nil
    end

    def flow_internal_content
      token = @scanner.peek_token
      if !(token.__is_value || token.__is_flow_entry || token.__is_flow_sequence_end)
        flow_node
      else
        empty_scalar
      end
    end

    def flow_internal_value
      if @scanner.peek_token.__is_value
        @scanner.get_token
        if !(@scanner.peek_token.__is_flow_entry || @scanner.peek_token.__is_flow_sequence_end)
          flow_node
        else
          empty_scalar
        end
      else
        empty_scalar
      end
    end

    def flow_entry_marker
      if @scanner.peek_token.__is_flow_entry
        @scanner.get_token
      end
      nil
    end

    def flow_mapping_entry
      if !@scanner.peek_token.__is_flow_mapping_end
        if @scanner.peek_token.__is_key
          @parse_stack += [:flow_mapping_entry,:flow_entry_marker,:flow_mapping_internal_value]
          return flow_mapping_internal_content
        else
          @parse_stack += [:flow_mapping_entry,:flow_node]
          return flow_entry_marker
        end
      end
      nil
    end

    def flow_mapping_internal_content
      curr = @scanner.peek_token
      if !(curr.__is_value || curr.__is_flow_entry || curr.__is_flow_mapping_end)
        @scanner.get_token
        flow_node
      else
        empty_scalar
      end
    end

    def flow_mapping_internal_value
      if @scanner.peek_token.__is_value
        @scanner.get_token
        if !(@scanner.peek_token.__is_flow_entry || @scanner.peek_token.__is_flow_mapping_end)
          flow_node
        else
          empty_scalar
        end
      else
        empty_scalar
      end
    end
   

    def process_directives
      # DIRECTIVE*
      while @scanner.peek_token.__is_directive
        token = @scanner.get_token
        if token.name == "YAML"
          raise ParserError.new(nil,"found duplicate YAML directive") if !@yaml_version.nil?
          major, minor = token.value[0].to_i, token.value[1].to_i
          raise ParserError.new(nil,"found incompatible YAML document (version 1.* is required)") if major != 1
          @yaml_version = [major,minor]
        elsif token.name == "TAG"
          handle, prefix = token.value
          raise ParserError.new(nil,"duplicate tag handle #{handle}") if @tag_handles.member?(handle)
          @tag_handles[handle] = prefix
        end
      end
      if !@tag_handles.empty?
        value = @yaml_version, @tag_handles.dup
      else
        value = @yaml_version, nil
      end
      for key in DEFAULT_TAGS.keys
        @tag_handles[key] = DEFAULT_TAGS[key] if !@tag_handles.include?(key)
      end
      value
    end

    def process_empty_scalar
      ScalarEvent.new(nil, nil, [true, false], "")
    end
  end
end

