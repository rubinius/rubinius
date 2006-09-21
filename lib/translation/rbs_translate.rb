require 'sydparse'
require 'translation/extractors'
require 'translation/normalize'
require 'translation/local_scoping'
require 'sexp/composite_processor'
require 'translation/states'
require 'translation/generate_header'
require 'translation/to_c'

class RubiniusTranslator
  def initialize(prefix="rbs_")
    @info = TypeInfo.new
    @typer = RsTyper.new(@info)
    @prefix = prefix
  end
  
  attr_reader :header, :body
  
  def load(code)    
    hinted = SydneyParser.unified_sexp code
    hints = ExtractHints.new
    hints.process hinted
    
    syd = SydneyParser.load_string code
    norm = @typer.normalize syd.sexp
    @typer.process norm
    @typer.process norm
  end
  
  def classes
    @info.classes
  end
  
  require 'pp'
    
  def translate_to_c
    classes.each do |name, klass|
      gh = GenerateHeader.new(klass, @info)
      gh.prefix = @prefix
      @header = gh.generate
      
      codes = [gh.generate_new_body]
      
      sorted = klass.functions.sort { |a,b| a[0].to_s <=> b[0].to_s }
      
      sorted.each do |fname, func|
        next unless func.body
        cg = TypeInfo::CodeGenerator.new(self)
        rc = RsToCProcessor.new(@info, cg)
        sbody = func.body
        sbody.unshift :block
        ending = t(:return, sbody[-1])
        ending.set_type sbody[-1].type
        sbody[-1] = ending
        body = rc.process(sbody)
        
        codes << gh.wrap_function(fname, func, body)
      end
      
      @body = codes.join("\n")
    end
  end
  
  def rehash_functions
    classes.each do |name, klass|
      klass.functions.each do |fname, func|
        next unless func.body
        ty = RsTyper.new(@info)
        ty.process_function klass, func
      end
    end
  end
  
  def load_types(kind="c")
    @info.add_c_type Type.Fixnum, "int"
    
    Dir["lib/translation/types/#{kind}/*.rb"].each do |path|
      @info.load_file path
    end
  end  
end