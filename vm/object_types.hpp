#ifndef RBX_VM_OBJECT_TYPES_HPP
#define RBX_VM_OBJECT_TYPES_HPP

namespace rubinius {
  /* rubinius_object types, takes up 3 bits */
  typedef enum
  {
    InvalidType     = 0,
    NumericType     ,
    IntegerType     ,
    FalseType       ,
    TrueType        ,
    NilType         ,
    ObjectType      ,
    MContextType    ,
    BContextType    ,
    ClassType       ,
    MetaclassType   ,
    MTType          ,
    WrapsStructType ,
    IncModType      ,
    TaskType        ,
    FixnumType      ,
    BignumType      ,
    FloatType       ,
    MemPtrType      ,
    StringType      ,
    SymbolType      ,
    CMethodType     ,
    NMethodType     ,
    BlockEnvType    ,
    TupleType       ,
    ArrayType       ,
    HashType        ,
    RegexpDataType  ,
    RegexpType      ,
    ByteArrayType   ,
    ISeqType        ,
    ChannelType     ,
    ThreadType      ,
    ModuleType      ,
    SendSiteType    ,
    SelectorType    ,
    LookupTableType ,
    IOBufferType    ,
    StaticScopeType ,
    SymbolTableType ,
    MatchDataType   ,
    ExceptionType   ,
    IOType          ,
    ExecutableType  ,
    CMVisibilityType,
    ListType        ,
    ListNodeType    ,
    NativeFuncType  ,

    LastObjectType   // must remain at end
  } object_type;
}

#endif
