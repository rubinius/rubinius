%"struct.rubinius::VM" = type opaque
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
%"struct.rubinius::StaticScope" = type opaque

%"struct.rubinius::Arguments" = type {
   %"struct.rubinius::Symbol"*, ; name
   %"struct.rubinius::Object"*, ; recv
   %"struct.rubinius::Object"*, ; block
                           i32, ; total
  %"struct.rubinius::Object"**, ; arguments
     %"struct.rubinius::Tuple"* ; argument_container
}

%"struct.rubinius::jit::RuntimeData" = type {
  %"struct.rubinius::CompiledMethod"*, ; method
          %"struct.rubinius::Symbol"*, ; name
          %"struct.rubinius::Module"*  ; module
}

%"struct.rubinius::Dispatch" = type {
      %"struct.rubinius::Symbol"*, ; name
      %"struct.rubinius::Module"*, ; module
  %"struct.rubinius::Executable"*, ; method
                               i8  ; method_missing
}

%"struct.rubinius::InlineCache" = type {
            %"struct.rubinius::Symbol"*, ; name
  %"struct.rubinius::MethodCacheEntry"*, ; cache
          %"struct.rubinius::CallUnit"*, ; call_unit
  %"struct.rubinius::Object"* (%"struct.rubinius::VM"*, %"struct.rubinius::InlineCache"*, %"struct.rubinius::CallFrame"*, %"struct.rubinius::Arguments"*)*, ; initial
  %"struct.rubinius::Object"* (%"struct.rubinius::VM"*, %"struct.rubinius::InlineCache"*, %"struct.rubinius::CallFrame"*, %"struct.rubinius::Arguments"*)*, ; execute
                                      i32*, ; hits
                                       i32, ; seen_classes_overflow
  [3 x %"struct.rubinius::InlineCacheHit"], ; seen_classes
                                       i32  ; private_lock
}

%"struct.rubinius::InlineCacheHit" = type {
  %"struct.rubinius::Class"*, ; seen_class
                         i32  ; hits
}

%"struct.rubinius::StackVariables" = type {
  %"struct.rubinius::VariableScope"*, ; on_heap
  %"struct.rubinius::VariableScope"*, ; parent
         %"struct.rubinius::Object"*, ; self
         %"struct.rubinius::Object"*, ; block
         %"struct.rubinius::Module"*, ; module
         %"struct.rubinius::Object"*, ; last_match
   [0 x %"struct.rubinius::Object"*]  ; locals
}

%"struct.rubinius::UnwindInfo" = type {
  i32, ; target_ip
  i32, ; stack_depth
  i32  ; type
}


%"union.rubinius::HeaderWord" = type {
  i64 ; flags64
}

%"struct.rubinius::Object" = type {
  %"struct.rubinius::ObjectHeader" ; header
}

%"struct.rubinius::ObjectFlags" = type {
  i32, ; flags
  i32  ; aux_word
}

%"struct.rubinius::ObjectHeader" = type {
  %"union.rubinius::HeaderWord", ; header
     %"struct.rubinius::Class"*, ; klass
    %"struct.rubinius::Object"*, ; ivars
                      [0 x i8*]  ; body
}

%"struct.rubinius::Array" = type {
   %"struct.rubinius::Object", ; header
  %"struct.rubinius::Fixnum"*, ; total
   %"struct.rubinius::Tuple"*, ; tuple
  %"struct.rubinius::Fixnum"*, ; start
  %"struct.rubinius::Object"*  ; shared
}

%"struct.rubinius::BlockEnvironment" = type {
           %"struct.rubinius::Object", ; header
   %"struct.rubinius::VariableScope"*, ; scope
   %"struct.rubinius::VariableScope"*, ; top_scope
          %"struct.rubinius::Object"*, ; local_count
  %"struct.rubinius::CompiledMethod"*, ; method
          %"struct.rubinius::Module"*  ; module
}

%"struct.rubinius::BlockInvocation" = type {
                               i32, ; flags
       %"struct.rubinius::Object"*, ; self
  %"struct.rubinius::StaticScope"*, ; static_scope
       %"struct.rubinius::Module"*  ; module
}

%"struct.rubinius::CallFrame" = type {
       %"struct.rubinius::CallFrame"*, ; previous
     %"struct.rubinius::StaticScope"*, ; static_scope
                                  i8*, ; dispatch_data
  %"struct.rubinius::CompiledMethod"*, ; cm
                                  i32, ; flags
                                  i32, ; ip
                                  i8*, ; jit_data
   %"struct.rubinius::VariableScope"*, ; top_scope
  %"struct.rubinius::StackVariables"*, ; scope
       %"struct.rubinius::Arguments"*, ; arguments
    [0 x %"struct.rubinius::Object"*]  ; stk
}

%"struct.rubinius::Class" = type {
        %"struct.rubinius::Module", ; header
       %"struct.rubinius::Fixnum"*, ; instance_type
  %"struct.rubinius::LookupTable"*, ; packed_ivar_info
     %"struct.rubinius::TypeInfo"*, ; type_info
                               i32, ; class_id
                               i32, ; packed_size
                               i8   ; building
}

%"struct.rubinius::CompiledMethod" = type {
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
             %"struct.rubinius::StaticScope"*, ; scope
             %"struct.rubinius::LookupTable"*, ; breakpoints
                %"struct.rubinius::VMMethod"*, ; backend_method
  %"struct.rubinius::jit::RuntimeDataHolder"*, ; jit_data
                   %"struct.rubinius::Tuple"*  ; literals
}

%"struct.rubinius::Executable" = type {
   %"struct.rubinius::Object", ; header
  %"struct.rubinius::Symbol"*, ; primitive
  %"struct.rubinius::Fixnum"*, ; serial
  %"struct.rubinius::Object"* (%"struct.rubinius::VM"*, %"struct.rubinius::CallFrame"*, %"struct.rubinius::Executable"*, %"struct.rubinius::Module"*, %"struct.rubinius::Arguments"*)*, ; execute
                            i32, ; prim_index
  %"struct.rubinius::Inliners"*  ; inliners
}

%"struct.rubinius::Float" = type {
  %"struct.rubinius::Numeric", ; header
                       double  ; val
}

%"struct.rubinius::Module" = type {
        %"struct.rubinius::Object", ; header
  %"struct.rubinius::MethodTable"*, ; method_table
       %"struct.rubinius::Symbol"*, ; module_name
  %"struct.rubinius::LookupTable"*, ; constants
       %"struct.rubinius::Module"*, ; superclass
        %"struct.rubinius::Array"*  ; seen_ivars
}

%"struct.rubinius::Numeric" = type {
  %"struct.rubinius::Object" ; header
}

%"struct.rubinius::Tuple" = type {
         %"struct.rubinius::Object", ; header
                                i32, ; full_size
  [0 x %"struct.rubinius::Object"*]  ; field
}

%"struct.rubinius::VariableScope" = type {
           %"struct.rubinius::Object", ; header
          %"struct.rubinius::Object"*, ; block
  %"struct.rubinius::CompiledMethod"*, ; method
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

