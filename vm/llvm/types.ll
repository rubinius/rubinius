
%"struct.rubinius::State" = type opaque
%"struct.rubinius::TypeInfo" = type opaque
%"struct.rubinius::VMMethod" = type opaque
%"struct.rubinius::Fixnum" = type opaque
%"struct.rubinius::Symbol" = type opaque
%"struct.rubinius::Selector" = type opaque
%"struct.rubinius::LookupTable" = type opaque
%"struct.rubinius::MethodTable" = type opaque
%"struct.rubinius::jit::RuntimeDataHolder" = type opaque
%"struct.rubinius::Inliners" = type opaque
%"struct.rubinius::CallUnit" = type opaque
%"struct.rubinius::InstructionSequence" = type opaque
%"struct.rubinius::MethodCacheEntry" = type opaque
%"struct.rubinius::ConstantScope" = type opaque

%"struct.rubinius::Arguments" = type {
   %"struct.rubinius::Symbol"*, ; name
   %"struct.rubinius::Object"*, ; recv
   %"struct.rubinius::Object"*, ; block
                           i32, ; total
  %"struct.rubinius::Object"**, ; arguments
     %"struct.rubinius::Tuple"* ; argument_container
}

declare void @output1(%"struct.rubinius::Arguments"*)

%"struct.rubinius::jit::RuntimeData" = type {
  %"struct.rubinius::CompiledCode"*, ; method
          %"struct.rubinius::Symbol"*, ; name
          %"struct.rubinius::Module"*  ; module
}

declare void @output2(%"struct.rubinius::jit::RuntimeData"*)

%"struct.rubinius::Dispatch" = type {
      %"struct.rubinius::Symbol"*, ; name
      %"struct.rubinius::Module"*, ; module
  %"struct.rubinius::Executable"*, ; method
                               i8  ; method_missing
}

declare void @output3(%"struct.rubinius::Dispatch"*)

%"struct.rubinius::InlineCache" = type {
            %"struct.rubinius::Symbol"*, ; name
  [3 x %"struct.rubinius::InlineCacheHit"], ; cache
          %"struct.rubinius::CallUnit"*, ; call_unit
  %"struct.rubinius::Object"* (%"struct.rubinius::State"*, %"struct.rubinius::InlineCache"*, %"struct.rubinius::CallFrame"*, %"struct.rubinius::Arguments"*)*, ; initial
  %"struct.rubinius::Object"* (%"struct.rubinius::State"*, %"struct.rubinius::InlineCache"*, %"struct.rubinius::CallFrame"*, %"struct.rubinius::Arguments"*)*, ; execute
                                       i32 ; seen_classes_overflow
}

declare void @output4(%"struct.rubinius::InlineCache"*)

%"struct.rubinius::InlineCacheHit" = type {
  %"struct.rubinius::MethodCacheEntry"*, ; entry
                         i32  ; hits
}

declare void @output5(%"struct.rubinius::InlineCacheHit"*)

%"struct.rubinius::StackVariables" = type {
  %"struct.rubinius::VariableScope"*, ; on_heap
  %"struct.rubinius::VariableScope"*, ; parent
         %"struct.rubinius::Object"*, ; self
         %"struct.rubinius::Object"*, ; block
         %"struct.rubinius::Module"*, ; module
         %"struct.rubinius::Object"*, ; last_match
   [0 x %"struct.rubinius::Object"*]  ; locals
}

declare void @output6(%"struct.rubinius::StackVariables"*)

%"struct.rubinius::UnwindInfo" = type {
  i32, ; target_ip
  i32, ; stack_depth
  i32  ; type
}

declare void @output7(%"struct.rubinius::UnwindInfo"*)

%"union.rubinius::HeaderWord" = type {
  i64 ; flags64
}

declare void @output8(%"union.rubinius::HeaderWord"*)

%"struct.rubinius::Object" = type {
  %"struct.rubinius::ObjectHeader" ; header
}

declare void @output9(%"struct.rubinius::Object"*)

%"struct.rubinius::ObjectFlags" = type {
  i32, ; flags
  i32  ; aux_word
}

declare void @output10(%"struct.rubinius::ObjectFlags"*)

%"struct.rubinius::ObjectHeader" = type {
  %"union.rubinius::HeaderWord", ; header
     %"struct.rubinius::Class"*, ; klass
    %"struct.rubinius::Object"*, ; ivars
                      [0 x i8*]  ; body
}

declare void @output11(%"struct.rubinius::ObjectHeader"*)

%"struct.rubinius::Array" = type {
   %"struct.rubinius::Object", ; header
  %"struct.rubinius::Fixnum"*, ; total
   %"struct.rubinius::Tuple"*, ; tuple
  %"struct.rubinius::Fixnum"*, ; start
  %"struct.rubinius::Object"*  ; shared
}

declare void @output12(%"struct.rubinius::Array"*)

%"struct.rubinius::BlockEnvironment" = type {
           %"struct.rubinius::Object", ; header
   %"struct.rubinius::VariableScope"*, ; scope
   %"struct.rubinius::VariableScope"*, ; top_scope
  %"struct.rubinius::CompiledCode"*, ; code
          %"struct.rubinius::Module"*  ; module
}

declare void @output13(%"struct.rubinius::BlockEnvironment"*)

%"struct.rubinius::BlockInvocation" = type {
                                 i32, ; flags
         %"struct.rubinius::Object"*, ; self
  %"struct.rubinius::ConstantScope"*, ; constant_scope
         %"struct.rubinius::Module"*  ; module
}

declare void @output14(%"struct.rubinius::BlockInvocation"*)

%"struct.rubinius::CallFrame" = type {
       %"struct.rubinius::CallFrame"*, ; previous
   %"struct.rubinius::ConstantScope"*, ; constant_scope
                                  i8*, ; dispatch_data
  %"struct.rubinius::CompiledCode"*, ; cm
                                  i32, ; flags
                                  i32, ; ip
                                  i8*, ; jit_data
   %"struct.rubinius::VariableScope"*, ; top_scope
  %"struct.rubinius::StackVariables"*, ; scope
       %"struct.rubinius::Arguments"*, ; arguments
    [0 x %"struct.rubinius::Object"*]  ; stk
}

declare void @output15(%"struct.rubinius::CallFrame"*)

%"struct.rubinius::Class" = type {
        %"struct.rubinius::Module", ; header
       %"struct.rubinius::Fixnum"*, ; instance_type
  %"struct.rubinius::LookupTable"*, ; packed_ivar_info
     %"struct.rubinius::TypeInfo"*, ; type_info
                               i32, ; class_id
                               i32, ; packed_size
                               i8   ; building
}

declare void @output16(%"struct.rubinius::Class"*)

%"struct.rubinius::CompiledCode" = type {
               %"struct.rubinius::Executable", ; header
                  %"struct.rubinius::Object"*, ; metadata
                  %"struct.rubinius::Symbol"*, ; name
     %"struct.rubinius::InstructionSequence"*, ; iseq
                  %"struct.rubinius::Fixnum"*, ; stack_size
                  %"struct.rubinius::Fixnum"*, ; local_count
                  %"struct.rubinius::Fixnum"*, ; required_args
                  %"struct.rubinius::Fixnum"*, ; post_args
                  %"struct.rubinius::Fixnum"*, ; total_args
                  %"struct.rubinius::Object"*, ; splat
                   %"struct.rubinius::Tuple"*, ; lines
                   %"struct.rubinius::Tuple"*, ; local_names
                  %"struct.rubinius::Symbol"*, ; file
           %"struct.rubinius::ConstantScope"*, ; scope
             %"struct.rubinius::LookupTable"*, ; breakpoints
                %"struct.rubinius::VMMethod"*, ; backend_method
  %"struct.rubinius::jit::RuntimeDataHolder"*, ; jit_data
                   %"struct.rubinius::Tuple"*  ; literals
}

declare void @output17(%"struct.rubinius::CompiledCode"*)

%"struct.rubinius::Executable" = type {
   %"struct.rubinius::Object", ; header
  %"struct.rubinius::Symbol"*, ; primitive
  %"struct.rubinius::Fixnum"*, ; serial
  %"struct.rubinius::Object"* (%"struct.rubinius::State"*, %"struct.rubinius::CallFrame"*, %"struct.rubinius::Executable"*, %"struct.rubinius::Module"*, %"struct.rubinius::Arguments"*)*, ; execute
                            i32, ; prim_index
  %"struct.rubinius::Inliners"*  ; inliners
}

declare void @output18(%"struct.rubinius::Executable"*)

%"struct.rubinius::Float" = type {
  %"struct.rubinius::Numeric", ; header
                       double  ; val
}

declare void @output19(%"struct.rubinius::Float"*)

%"struct.rubinius::Module" = type {
        %"struct.rubinius::Object", ; header
  %"struct.rubinius::MethodTable"*, ; method_table
       %"struct.rubinius::Symbol"*, ; module_name
  %"struct.rubinius::LookupTable"*, ; constants
       %"struct.rubinius::Module"*, ; superclass
        %"struct.rubinius::Array"*  ; seen_ivars
}

declare void @output20(%"struct.rubinius::Module"*)

%"struct.rubinius::Numeric" = type {
  %"struct.rubinius::Object" ; header
}

declare void @output21(%"struct.rubinius::Numeric"*)

%"struct.rubinius::Tuple" = type {
         %"struct.rubinius::Object", ; header
                                i32, ; full_size
  [0 x %"struct.rubinius::Object"*]  ; field
}

declare void @output22(%"struct.rubinius::Tuple"*)

%"struct.rubinius::VariableScope" = type {
           %"struct.rubinius::Object", ; header
          %"struct.rubinius::Object"*, ; block
  %"struct.rubinius::CompiledCode"*, ; method
          %"struct.rubinius::Module"*, ; module
   %"struct.rubinius::VariableScope"*, ; parent
           %"struct.rubinius::Tuple"*, ; heap_locals
          %"struct.rubinius::Object"*, ; last_match
          %"struct.rubinius::Object"*, ; self
                                  i32, ; number_of_locals
                                   i8, ; isolated
         %"struct.rubinius::Object"**, ; locals
                                  i32  ; block_as_method
}

declare void @output23(%"struct.rubinius::VariableScope"*)

