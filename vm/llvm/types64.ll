
%"struct.rubinius::VM" = type opaque
%"struct.rubinius::Shared" = type opaque
%"struct.rubinius::TypeInfo" = type opaque
%"struct.rubinius::MachineCode" = type opaque
%"struct.rubinius::Fixnum" = type opaque
%"struct.rubinius::Symbol" = type opaque
%"struct.rubinius::Selector" = type opaque
%"struct.rubinius::LookupTable" = type opaque
%"struct.rubinius::MethodTable" = type opaque
%"struct.rubinius::jit::RuntimeDataHolder" = type opaque
%"struct.rubinius::Inliners" = type opaque
%"struct.rubinius::CallUnit" = type opaque
%"struct.rubinius::InstructionSequence" = type opaque
%"struct.rubinius::ConstantScope" = type opaque
%"struct.rubinius::Fiber" = type opaque

%"struct.rubinius::VMJIT" = type {
  i64, ; stack_start
  i64, ; stack_limit
  i32, ; stack_size
   i8, ; interrupt_with_signal
   i8, ; interrupt_by_kill
   i8, ; check_local_interrupts
   i8  ; thread_step
}

declare void @outputVMJIT(%"struct.rubinius::VMJIT"*)

%"struct.rubinius::State" = type {
   %"struct.rubinius::VM"*,     ; vm
   %"struct.rubinius::VMJIT"*,  ; vm_jit
   %"struct.rubinius::Shared"*  ; shared
}

declare void @outputState(%"struct.rubinius::State"*)

%"struct.rubinius::Arguments" = type {
   %"struct.rubinius::Symbol"*, ; name
   %"struct.rubinius::Object"*, ; recv
   %"struct.rubinius::Object"*, ; block
                           i32, ; total
  %"struct.rubinius::Object"**, ; arguments
     %"struct.rubinius::Tuple"* ; argument_container
}

declare void @outputArguments(%"struct.rubinius::Arguments"*)

%"struct.rubinius::jit::RuntimeData" = type {
  %"struct.rubinius::CompiledCode"*, ; method
          %"struct.rubinius::Symbol"*, ; name
          %"struct.rubinius::Module"*  ; module
}

declare void @outputRuntimeData(%"struct.rubinius::jit::RuntimeData"*)

%"struct.rubinius::Dispatch" = type {
      %"struct.rubinius::Symbol"*, ; name
      %"struct.rubinius::Module"*, ; module
  %"struct.rubinius::Executable"*, ; method
      %"struct.rubinius::Symbol"*, ; visibility
                              i32  ; method_missing
}

declare void @outputDispatch(%"struct.rubinius::Dispatch"*)

%"struct.rubinius::CallSite" = type {
       %"struct.rubinius::Object", ; header
      %"struct.rubinius::Symbol"*, ; name
  %"struct.rubinius::Object"* (%"struct.rubinius::State"*, %"struct.rubinius::CallSite"*, %"struct.rubinius::CallFrame"*, %"struct.rubinius::Arguments"*)*, ; executor
  %"struct.rubinius::Object"* (%"struct.rubinius::State"*, %"struct.rubinius::CallSite"*, %"struct.rubinius::CallFrame"*, %"struct.rubinius::Arguments"*)*, ; fallback
  void (%"struct.rubinius::State"*, %"struct.rubinius::CallSite"*, %"struct.rubinius::Class"*, %"struct.rubinius::Dispatch"*)*, ; updater
  %"struct.rubinius::Executable"*, ; executable
                              i32  ; ip
}

declare void @outputCallSite(%"struct.rubinius::CallSite"*)

%"struct.rubinius::MonoInlineCache" = type {
    %"struct.rubinius::CallSite",  ; header
                              i32, ; receiver_class_id
                              i32, ; receiver_serial_id
       %"struct.rubinius::Class"*, ; receiver_class
      %"struct.rubinius::Module"*, ; stored_module
  %"struct.rubinius::Executable"*, ; method
                              i32, ; method_missing
                              i32  ; hits
}

declare void @outputMonoInlineCache(%"struct.rubinius::MonoInlineCache"*)

%"struct.rubinius::PolyInlineCache" = type {
             %"struct.rubinius::CallSite",  ; header
  [3 x %"struct.rubinius::InlineCacheEntry"*], ; entries
                                       i32  ; seen_classes_overflow
}

declare void @outputPolyInlineCache(%"struct.rubinius::PolyInlineCache"*)

%"struct.rubinius::InlineCacheEntry" = type {
   %"struct.rubinius::Object", ; header
  %"struct.rubinius::Module"*, ; stored_module
   %"struct.rubinius::Class"*, ; receiver_class
  %"struct.rubinius::Executable"*, ; method
                          i32, ; receiver_class_id
                          i32, ; receiver_serial_id
                          i32, ; method_missing
                          i32  ; hits
}

declare void @outputInlineCacheEntry(%"struct.rubinius::InlineCacheEntry"*)

%"struct.rubinius::ConstantCache" = type {
               %"struct.rubinius::Object", ; header
              %"struct.rubinius::Symbol"*, ; name
              %"struct.rubinius::Object"*, ; value
              %"struct.rubinius::Module"*, ; under
       %"struct.rubinius::ConstantScope"*, ; scope
          %"struct.rubinius::Executable"*, ; executable
                                      i32, ; ip
                                      i32  ; serial
}

declare void @outputConstantCache(%"struct.rubinius::ConstantCache"*)

%"struct.rubinius::StackVariables" = type {
  %"struct.rubinius::VariableScope"*, ; on_heap
  %"struct.rubinius::VariableScope"*, ; parent
         %"struct.rubinius::Object"*, ; self
         %"struct.rubinius::Object"*, ; block
         %"struct.rubinius::Module"*, ; module
         %"struct.rubinius::Object"*, ; last_match
   [0 x %"struct.rubinius::Object"*]  ; locals
}

declare void @outputStackVariables(%"struct.rubinius::StackVariables"*)

%"struct.rubinius::UnwindInfo" = type {
  i32, ; target_ip
  i32, ; stack_depth
  i32  ; type
}

declare void @outputUnwindInfo(%"struct.rubinius::UnwindInfo"*)

%"union.rubinius::HeaderWord" = type {
  i64 ; flags64
}

declare void @outputHeaderWord(%"union.rubinius::HeaderWord"*)

%"struct.rubinius::Object" = type {
  %"struct.rubinius::ObjectHeader" ; header
}

declare void @outputObject(%"struct.rubinius::Object"*)

%"struct.rubinius::ObjectFlags" = type {
  i32, ; flags
  i32  ; aux_word
}

declare void @outputObjectFlags(%"struct.rubinius::ObjectFlags"*)

%"struct.rubinius::ObjectHeader" = type {
  %"union.rubinius::HeaderWord", ; header
     %"struct.rubinius::Class"*, ; klass
    %"struct.rubinius::Object"*, ; ivars
                      [0 x i8*]  ; body
}

declare void @outputObjectHeader(%"struct.rubinius::ObjectHeader"*)

%"struct.rubinius::Array" = type {
   %"struct.rubinius::Object", ; header
  %"struct.rubinius::Fixnum"*, ; total
   %"struct.rubinius::Tuple"*, ; tuple
  %"struct.rubinius::Fixnum"*  ; start
}

declare void @outputArray(%"struct.rubinius::Array"*)

%"struct.rubinius::BlockEnvironment" = type {
           %"struct.rubinius::Object", ; header
   %"struct.rubinius::VariableScope"*, ; scope
   %"struct.rubinius::VariableScope"*, ; top_scope
  %"struct.rubinius::CompiledCode"*,   ; code
  %"struct.rubinius::ConstantScope"*,  ; constant_scope
          %"struct.rubinius::Module"*  ; module
}

declare void @outputBlockEnvironment(%"struct.rubinius::BlockEnvironment"*)

%"struct.rubinius::BlockInvocation" = type {
                                 i32, ; flags
         %"struct.rubinius::Object"*, ; self
  %"struct.rubinius::ConstantScope"*, ; constant_scope
         %"struct.rubinius::Module"*  ; module
}

declare void @outputBlockInvocation(%"struct.rubinius::BlockInvocation"*)

%"struct.rubinius::CallFrame" = type {
       %"struct.rubinius::CallFrame"*, ; previous
   %"struct.rubinius::ConstantScope"*, ; constant_scope
                                  i8*, ; dispatch_data
    %"struct.rubinius::CompiledCode"*, ; compiled_code
                                  i32, ; flags
                                  i32, ; ip
                                  i8*, ; jit_data
   %"struct.rubinius::VariableScope"*, ; top_scope
  %"struct.rubinius::StackVariables"*, ; scope
       %"struct.rubinius::Arguments"*, ; arguments
    [0 x %"struct.rubinius::Object"*]  ; stk
}

declare void @outputCallFrame(%"struct.rubinius::CallFrame"*)

%"struct.rubinius::Class" = type {
        %"struct.rubinius::Module", ; header
       %"struct.rubinius::Fixnum"*, ; instance_type
  %"struct.rubinius::LookupTable"*, ; packed_ivar_info
     %"struct.rubinius::TypeInfo"*, ; type_info
                               i32, ; class_id
                               i32, ; serial_id
                               i32  ; packed_size
}

declare void @outputClass(%"struct.rubinius::Class"*)

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
                  %"struct.rubinius::Fixnum"*, ; splat
                   %"struct.rubinius::Tuple"*, ; lines
                   %"struct.rubinius::Tuple"*, ; local_names
                  %"struct.rubinius::Symbol"*, ; file
           %"struct.rubinius::ConstantScope"*, ; scope
                   %"struct.rubinius::Tuple"*, ; keywords
                  %"struct.rubinius::Fixnum"*, ; arity
             %"struct.rubinius::LookupTable"*, ; breakpoints
             %"struct.rubinius::MachineCode"*, ; machine_code
  %"struct.rubinius::jit::RuntimeDataHolder"*, ; jit_data
                   %"struct.rubinius::Tuple"*  ; literals
}

declare void @outputCompiledCode(%"struct.rubinius::CompiledCode"*)

%"struct.rubinius::Executable" = type {
   %"struct.rubinius::Object", ; header
  %"struct.rubinius::Symbol"*, ; primitive
  %"struct.rubinius::Fixnum"*, ; serial
  %"struct.rubinius::Object"* (%"struct.rubinius::State"*, %"struct.rubinius::CallFrame"*, %"struct.rubinius::Executable"*, %"struct.rubinius::Module"*, %"struct.rubinius::Arguments"*)*, ; execute
  %"struct.rubinius::Inliners"*, ; inliners
                            i32, ; prim_index
                             i8  ; custom_call_site
}

declare void @outputExecutable(%"struct.rubinius::Executable"*)

%"struct.rubinius::Float" = type {
  %"struct.rubinius::Numeric", ; header
                       double  ; val
}

declare void @outputFloat(%"struct.rubinius::Float"*)

%"struct.rubinius::Module" = type {
        %"struct.rubinius::Object", ; header
  %"struct.rubinius::MethodTable"*, ; method_table
       %"struct.rubinius::Symbol"*, ; module_name
  %"struct.rubinius::LookupTable"*, ; constants
       %"struct.rubinius::Module"*, ; superclass
       %"struct.rubinius::Module"*, ; origin
        %"struct.rubinius::Array"*, ; seen_ivars
        %"struct.rubinius::Class"*, ; mirror
        %"struct.rubinius::Array"*  ; subclasses
}

declare void @outputModule(%"struct.rubinius::Module"*)

%"struct.rubinius::Numeric" = type {
  %"struct.rubinius::Object" ; header
}

declare void @outputNumeric(%"struct.rubinius::Numeric"*)

%"struct.rubinius::Tuple" = type {
         %"struct.rubinius::Object", ; header
                                i64, ; full_size
  [0 x %"struct.rubinius::Object"*]  ; field
}

declare void @outputTuple(%"struct.rubinius::Tuple"*)

%"struct.rubinius::VariableScope" = type {
           %"struct.rubinius::Object", ; header
          %"struct.rubinius::Object"*, ; block
  %"struct.rubinius::CompiledCode"*, ; method
          %"struct.rubinius::Module"*, ; module
   %"struct.rubinius::VariableScope"*, ; parent
           %"struct.rubinius::Tuple"*, ; heap_locals
     %"struct.rubinius::LookupTable"*, ; dynamic_locals
          %"struct.rubinius::Object"*, ; last_match
           %"struct.rubinius::Fiber"*, ; fiber
          %"struct.rubinius::Object"*, ; self
         %"struct.rubinius::Object"**, ; locals
                                  i32, ; number_of_locals
                                  i32, ; isolated
                                  i32  ; flags
}

declare void @outputVariableScope(%"struct.rubinius::VariableScope"*)

%"struct.rubinius::ByteArray" = type {
         %"struct.rubinius::Object", ; header
                                i64, ; full_size
                           [0 x i8]  ; field
}

declare void @outputByteArray(%"struct.rubinius::ByteArray"*)

%"struct.rubinius::Proc" = type {
  %"struct.rubinius::BlockEnvironment", ; block
            %"struct.rubinius::Object", ; lambda
            %"struct.rubinius::Object"  ; bound_method
}

declare void @outputProc(%"struct.rubinius::Proc"*)
